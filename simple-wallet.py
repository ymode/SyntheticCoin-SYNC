#!/usr/bin/env python3
"""Simple SYNC wallet without external dependencies"""

import hashlib
import sys
import os
import json

def create_address(device_name="Bitaxe001", network="testnet"):
    """Create a SYNC address from device name"""
    # Create deterministic address
    seed = f"SYNC:BITAXE:{device_name}:{network}"
    hash_bytes = hashlib.sha256(seed.encode()).hexdigest()
    
    # Format as SYNC address
    prefix = "tsync1q" if network == "testnet" else "sync1q"
    address = f"{prefix}{hash_bytes[:16]}"
    
    return address

def save_address(device_name, address):
    """Save address to wallet file"""
    wallet_dir = os.path.expanduser("~/.sync-testnet")
    os.makedirs(wallet_dir, exist_ok=True)
    
    wallet_file = os.path.join(wallet_dir, "wallet.json")
    
    # Load existing wallet
    if os.path.exists(wallet_file):
        with open(wallet_file, 'r') as f:
            wallet = json.load(f)
    else:
        wallet = {"addresses": {}}
    
    # Add new address
    wallet["addresses"][device_name] = {
        "address": address,
        "balance": 0,
        "type": "bitaxe",
        "multiplier": 2.0
    }
    
    # Save wallet
    with open(wallet_file, 'w') as f:
        json.dump(wallet, f, indent=2)
    
    return wallet_file

if __name__ == "__main__":
    device_name = sys.argv[1] if len(sys.argv) > 1 else "Bitaxe001"
    
    # Create address
    address = create_address(device_name)
    wallet_file = save_address(device_name, address)
    
    print(f"Address created: {address}")
    print(f"Device: {device_name}")
    print(f"Saved to: {wallet_file}")
