// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#include "device_verifier.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

namespace PoDD {

// Helper function to convert bytes to hex string
std::string BytesToHex(const uint8_t* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

// DeviceFingerprint implementation
double DeviceFingerprint::CalculateSimilarity(const DeviceFingerprint& other) const {
    double similarity = 0.0;
    double weight_sum = 0.0;
    
    // Timing similarity (most important - 40% weight)
    double timing_diff = std::abs(static_cast<double>(avg_nonce_time_us - other.avg_nonce_time_us));
    double timing_similarity = std::exp(-timing_diff / 10000.0); // Exponential decay
    similarity += timing_similarity * 0.4;
    weight_sum += 0.4;
    
    // Variance similarity (20% weight)
    double variance_diff = std::abs(static_cast<double>(timing_variance_us - other.timing_variance_us));
    double variance_similarity = std::exp(-variance_diff / 5000.0);
    similarity += variance_similarity * 0.2;
    weight_sum += 0.2;
    
    // Network similarity (20% weight)
    if (ip_address == other.ip_address) {
        // Same IP is suspicious but not conclusive
        similarity += 0.15;
    }
    double latency_diff = std::abs(static_cast<double>(avg_latency_ms - other.avg_latency_ms));
    double latency_similarity = std::exp(-latency_diff / 50.0);
    similarity += latency_similarity * 0.05;
    weight_sum += 0.2;
    
    // Hardware similarity (20% weight)
    if (firmware_version == other.firmware_version) {
        similarity += 0.05; // Same firmware is common
    }
    if (chip_count == other.chip_count) {
        similarity += 0.05; // Same chip count could be same model
    }
    double power_diff = std::abs(power_consumption_watts - other.power_consumption_watts);
    double power_similarity = std::exp(-power_diff / 10.0);
    similarity += power_similarity * 0.1;
    weight_sum += 0.2;
    
    return similarity / weight_sum;
}

std::string DeviceFingerprint::GetFingerprintHash() const {
    // Create a unique hash from device characteristics
    std::stringstream data;
    data << device_id << "|"
         << avg_nonce_time_us << "|"
         << timing_variance_us << "|"
         << firmware_version << "|"
         << chip_count << "|"
         << memory_size_mb;
    
    // Calculate SHA256 hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.str().c_str()), 
           data.str().length(), hash);
    
    return BytesToHex(hash, SHA256_DIGEST_LENGTH);
}

// MiningSquad implementation
bool MiningSquad::AddDevice(const std::string& device_id) {
    if (member_devices.size() >= 10) {
        return false; // Max squad size
    }
    
    if (std::find(member_devices.begin(), member_devices.end(), device_id) 
        != member_devices.end()) {
        return false; // Already in squad
    }
    
    member_devices.push_back(device_id);
    return true;
}

bool MiningSquad::RemoveDevice(const std::string& device_id) {
    auto it = std::find(member_devices.begin(), member_devices.end(), device_id);
    if (it != member_devices.end()) {
        member_devices.erase(it);
        return true;
    }
    return false;
}

double MiningSquad::GetRewardShare(const std::string& device_id) const {
    if (member_devices.empty()) return 0.0;
    
    auto it = std::find(member_devices.begin(), member_devices.end(), device_id);
    if (it == member_devices.end()) {
        return 0.0;
    }
    
    // Equal share for now, could be weighted by contribution later
    return 1.0 / member_devices.size();
}

// DeviceVerifier implementation
struct DeviceVerifier::Impl {
    // Timing entropy calculation
    double CalculateTimingEntropy(const std::vector<uint64_t>& timings) {
        if (timings.size() < 2) return 0.0;
        
        // Calculate probability distribution
        std::map<uint64_t, int> histogram;
        for (auto t : timings) {
            histogram[t / 1000]++; // Group by millisecond
        }
        
        double entropy = 0.0;
        double n = timings.size();
        for (const auto& [bucket, count] : histogram) {
            double p = count / n;
            if (p > 0) {
                entropy -= p * std::log2(p);
            }
        }
        
        return entropy;
    }
    
    // Check if timings are too synchronized (indicates single source)
    bool DetectSynchronizedTiming(const std::vector<DeviceFingerprint>& devices) {
        if (devices.size() < 2) return false;
        
        // Collect all timing samples
        std::vector<uint64_t> all_timings;
        for (const auto& device : devices) {
            for (auto timing : device.timing_samples) {
                if (timing > 0) all_timings.push_back(timing);
            }
        }
        
        if (all_timings.empty()) return false;
        
        // Calculate coefficient of variation
        double mean = std::accumulate(all_timings.begin(), all_timings.end(), 0.0) / all_timings.size();
        double sq_sum = 0.0;
        for (auto timing : all_timings) {
            sq_sum += std::pow(timing - mean, 2);
        }
        double stdev = std::sqrt(sq_sum / all_timings.size());
        double cv = stdev / mean;
        
        // Low coefficient of variation suggests synchronized source
        return cv < 0.1; // Less than 10% variation is suspicious
    }
};

DeviceVerifier::DeviceVerifier() : pImpl(std::make_unique<Impl>()) {}
DeviceVerifier::~DeviceVerifier() = default;

bool DeviceVerifier::RegisterDevice(const std::string& device_id, 
                                   const DeviceFingerprint& initial_fingerprint) {
    // Check if device already registered
    if (m_devices.find(device_id) != m_devices.end()) {
        return false;
    }
    
    // Store device fingerprint
    m_devices[device_id] = initial_fingerprint;
    m_registration_times[device_id] = std::chrono::steady_clock::now();
    
    return true;
}

void DeviceVerifier::UpdateDeviceFingerprint(const std::string& device_id,
                                            const ShareData& share_data) {
    auto it = m_devices.find(device_id);
    if (it == m_devices.end()) {
        return; // Device not registered
    }
    
    DeviceFingerprint& fp = it->second;
    
    // Update timing samples (rolling window)
    for (int i = 9; i > 0; --i) {
        fp.timing_samples[i] = fp.timing_samples[i-1];
    }
    fp.timing_samples[0] = share_data.timestamp_us;
    
    // Update average timing
    uint64_t sum = 0;
    int count = 0;
    for (auto sample : fp.timing_samples) {
        if (sample > 0) {
            sum += sample;
            count++;
        }
    }
    if (count > 0) {
        fp.avg_nonce_time_us = sum / count;
    }
    
    // Update other metrics
    fp.recent_nonces.push_back(share_data.nonce);
    if (fp.recent_nonces.size() > 100) {
        fp.recent_nonces.erase(fp.recent_nonces.begin());
    }
    
    fp.temperature_celsius = share_data.temperature;
    fp.power_consumption_watts = share_data.power_watts;
    fp.average_hashrate = share_data.hashrate;
    fp.last_seen = std::chrono::steady_clock::now();
    
    // Update network info if changed
    if (!share_data.ip_address.empty()) {
        fp.ip_address = share_data.ip_address;
    }
    if (share_data.latency_ms > 0) {
        // Rolling average for latency
        fp.avg_latency_ms = (fp.avg_latency_ms * 0.9) + (share_data.latency_ms * 0.1);
    }
}

DeviceVerifier::VerificationResult DeviceVerifier::VerifyDeviceDistribution(
    const std::vector<std::string>& device_ids) {
    
    VerificationResult result;
    result.is_valid = true;
    result.confidence = 1.0;
    
    if (device_ids.size() < 2) {
        result.reason = "Single device, no distribution to verify";
        return result;
    }
    
    // Collect fingerprints
    std::vector<DeviceFingerprint> fingerprints;
    for (const auto& id : device_ids) {
        auto it = m_devices.find(id);
        if (it != m_devices.end()) {
            fingerprints.push_back(it->second);
        }
    }
    
    if (fingerprints.size() < 2) {
        result.is_valid = false;
        result.confidence = 0.0;
        result.reason = "Not enough registered devices";
        return result;
    }
    
    // Check 1: Timing synchronization
    if (pImpl->DetectSynchronizedTiming(fingerprints)) {
        result.is_valid = false;
        result.confidence -= 0.4;
        result.reason = "Timing patterns too synchronized";
    }
    
    // Check 2: Similarity between devices
    for (size_t i = 0; i < fingerprints.size(); ++i) {
        for (size_t j = i + 1; j < fingerprints.size(); ++j) {
            double similarity = fingerprints[i].CalculateSimilarity(fingerprints[j]);
            if (similarity > 0.9) {
                result.is_valid = false;
                result.confidence -= 0.3;
                result.suspicious_pairs.push_back({device_ids[i], device_ids[j]});
                result.reason = "Devices too similar (likely same hardware)";
            }
        }
    }
    
    // Check 3: Network diversity
    std::set<std::string> unique_ips;
    for (const auto& fp : fingerprints) {
        unique_ips.insert(fp.ip_address);
    }
    
    double ip_diversity = static_cast<double>(unique_ips.size()) / fingerprints.size();
    if (ip_diversity < 0.5) {
        result.confidence -= 0.2;
        if (result.confidence < 0.5) {
            result.is_valid = false;
        }
        result.reason += "; Low network diversity";
    }
    
    // Check 4: Timing entropy
    std::vector<uint64_t> all_timings;
    for (const auto& fp : fingerprints) {
        for (auto t : fp.timing_samples) {
            if (t > 0) all_timings.push_back(t);
        }
    }
    
    double entropy = pImpl->CalculateTimingEntropy(all_timings);
    if (entropy < 2.0) { // Low entropy suggests artificial generation
        result.confidence -= 0.2;
        if (result.confidence < 0.5) {
            result.is_valid = false;
        }
        result.reason += "; Low timing entropy";
    }
    
    // Ensure confidence is in valid range
    result.confidence = std::max(0.0, std::min(1.0, result.confidence));
    
    return result;
}

bool DeviceVerifier::DetectSpoofing(const std::vector<std::string>& device_ids) {
    auto verification = VerifyDeviceDistribution(device_ids);
    return !verification.is_valid && verification.confidence < 0.3;
}

double DeviceVerifier::GetDeviceRewardMultiplier(const std::string& device_id) const {
    auto it = m_devices.find(device_id);
    if (it == m_devices.end()) {
        return 1.0; // No bonus for unregistered devices
    }
    
    // Check device registration age (anti-gaming)
    auto reg_it = m_registration_times.find(device_id);
    if (reg_it != m_registration_times.end()) {
        auto age = std::chrono::steady_clock::now() - reg_it->second;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(age).count();
        
        if (hours < 24) {
            return 1.0; // No bonus for very new devices
        }
    }
    
    // Base multiplier for verified device
    double multiplier = 1.1; // 10% bonus
    
    // Additional bonus for consistent mining
    const auto& fp = it->second;
    auto time_since_last = std::chrono::steady_clock::now() - fp.last_seen;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_since_last).count();
    
    if (minutes < 10) {
        multiplier += 0.05; // 5% bonus for active mining
    }
    
    // Efficiency bonus (hashes per watt)
    if (fp.power_consumption_watts > 0 && fp.average_hashrate > 0) {
        double efficiency = fp.average_hashrate / fp.power_consumption_watts;
        if (efficiency > 100) { // GH/s per watt
            multiplier += 0.05; // 5% efficiency bonus
        }
    }
    
    return multiplier;
}

std::string DeviceVerifier::FormMiningSquad(const std::vector<std::string>& device_ids) {
    if (device_ids.size() < 2 || device_ids.size() > 10) {
        return ""; // Invalid squad size
    }
    
    // Verify all devices are registered
    for (const auto& id : device_ids) {
        if (m_devices.find(id) == m_devices.end()) {
            return ""; // Unregistered device
        }
    }
    
    // Verify devices are genuinely different
    if (DetectSpoofing(device_ids)) {
        return ""; // Spoofing detected
    }
    
    // Generate squad ID
    std::stringstream squad_id_stream;
    squad_id_stream << "SQUAD_" << std::chrono::steady_clock::now().time_since_epoch().count();
    std::string squad_id = squad_id_stream.str();
    
    // Create squad
    MiningSquad squad;
    squad.squad_id = squad_id;
    squad.member_devices = device_ids;
    squad.created_at = std::chrono::steady_clock::now();
    squad.total_hashrate = 0;
    squad.blocks_found = 0;
    
    // Calculate total hashrate
    for (const auto& id : device_ids) {
        squad.total_hashrate += GetDeviceHashrate(id);
    }
    
    m_squads[squad_id] = squad;
    
    return squad_id;
}

double DeviceVerifier::GetDeviceHashrate(const std::string& device_id) const {
    auto it = m_devices.find(device_id);
    if (it != m_devices.end()) {
        return it->second.average_hashrate;
    }
    return 0.0;
}

// DeviceRegistry implementation
DeviceRegistry& DeviceRegistry::GetInstance() {
    static DeviceRegistry instance;
    return instance;
}

bool DeviceRegistry::RegisterDevice(const DeviceRegistration& registration) {
    if (m_registrations.find(registration.device_id) != m_registrations.end()) {
        return false; // Already registered
    }
    
    m_registrations[registration.device_id] = registration;
    return true;
}

bool DeviceRegistry::VerifyDeviceOwnership(const std::string& device_id, 
                                           const std::string& owner_address) {
    auto it = m_registrations.find(device_id);
    if (it != m_registrations.end()) {
        return it->second.owner_address == owner_address;
    }
    return false;
}

std::vector<std::string> DeviceRegistry::GetOwnerDevices(const std::string& owner_address) {
    std::vector<std::string> devices;
    for (const auto& [device_id, registration] : m_registrations) {
        if (registration.owner_address == owner_address) {
            devices.push_back(device_id);
        }
    }
    return devices;
}

bool DeviceRegistry::TransferDevice(const std::string& device_id,
                                   const std::string& from_address,
                                   const std::string& to_address) {
    auto it = m_registrations.find(device_id);
    if (it != m_registrations.end() && it->second.owner_address == from_address) {
        it->second.owner_address = to_address;
        return true;
    }
    return false;
}

uint32_t DeviceRegistry::GetTotalRegisteredDevices() const {
    return m_registrations.size();
}

} // namespace PoDD
