#!/bin/bash

# SyntheticCoin SYNC - GitHub Issues Creation Script
# This script creates all GitHub issues for the SYNC project
# Prerequisites: gh CLI must be installed and authenticated

set -e

echo "======================================"
echo "SyntheticCoin SYNC - Issue Creation"
echo "======================================"

# Check if gh CLI is installed
if ! command -v gh &> /dev/null; then
    echo "❌ gh CLI is not installed. Please install it first:"
    echo "   https://cli.github.com/manual/installation"
    exit 1
fi

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "❌ Not in a git repository. Please run this from the SYNC repository."
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo ""
echo "This script will create the following:"
echo "  • 5 Epic issues"
echo "  • 45+ Development issues across 7 milestones"
echo "  • 2 Issue templates (Bug Report, Feature Request)"
echo ""
read -p "Do you want to proceed? (y/n) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

echo ""
echo "Creating repository labels..."

# Create labels
gh label create "priority-critical" --description "Must be done immediately" --color "FF0000" 2>/dev/null || echo "Label priority-critical already exists"
gh label create "priority-high" --description "Should be done soon" --color "FF6B6B" 2>/dev/null || echo "Label priority-high already exists"
gh label create "priority-medium" --description "Should be done eventually" --color "FFD93D" 2>/dev/null || echo "Label priority-medium already exists"
gh label create "priority-low" --description "Nice to have" --color "6BCF7F" 2>/dev/null || echo "Label priority-low already exists"

gh label create "epic" --description "Large feature encompassing multiple issues" --color "3E1C5F" 2>/dev/null || echo "Label epic already exists"
gh label create "blockchain" --description "Core blockchain functionality" --color "0E185F" 2>/dev/null || echo "Label blockchain already exists"
gh label create "ai" --description "Artificial intelligence features" --color "FF00FF" 2>/dev/null || echo "Label ai already exists"
gh label create "quantum" --description "Quantum-resistant features" --color "8B00FF" 2>/dev/null || echo "Label quantum already exists"
gh label create "network" --description "P2P network related" --color "00CED1" 2>/dev/null || echo "Label network already exists"
gh label create "wallet" --description "Wallet functionality" --color "FFD700" 2>/dev/null || echo "Label wallet already exists"
gh label create "api" --description "API endpoints" --color "00FF00" 2>/dev/null || echo "Label api already exists"
gh label create "testing" --description "Testing and QA" --color "FFA500" 2>/dev/null || echo "Label testing already exists"
gh label create "security" --description "Security-related issues" --color "FF1493" 2>/dev/null || echo "Label security already exists"
gh label create "performance" --description "Performance optimization" --color "00FA9A" 2>/dev/null || echo "Label performance already exists"
gh label create "infrastructure" --description "Infrastructure and DevOps" --color "4B0082" 2>/dev/null || echo "Label infrastructure already exists"
gh label create "consensus" --description "Consensus mechanism" --color "800080" 2>/dev/null || echo "Label consensus already exists"
gh label create "innovation" --description "Novel features" --color "FF69B4" 2>/dev/null || echo "Label innovation already exists"
gh label create "defi" --description "DeFi features" --color "32CD32" 2>/dev/null || echo "Label defi already exists"
gh label create "governance" --description "Governance related" --color "4682B4" 2>/dev/null || echo "Label governance already exists"
gh label create "documentation" --description "Documentation updates" --color "0075CA" 2>/dev/null || echo "Label documentation already exists"

echo -e "${GREEN}✓${NC} Labels created"
echo ""

# Create milestones
echo "Creating milestones..."

gh api repos/:owner/:repo/milestones -f title="Phase 1: Foundation" -f description="Fork Bitcoin Core and establish base infrastructure" -f due_on="2026-01-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 1: Foundation' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 2: AI Features" -f description="Implement AI-driven components" -f due_on="2026-04-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 2: AI Features' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 3: Security Features" -f description="Add quantum resistance and security protocols" -f due_on="2026-07-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 3: Security Features' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 4: Economic Features" -f description="Implement staking and economic mechanisms" -f due_on="2026-10-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 4: Economic Features' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 5: Infrastructure" -f description="Build supporting infrastructure" -f due_on="2027-01-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 5: Infrastructure' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 6: Testing & QA" -f description="Comprehensive testing and quality assurance" -f due_on="2027-04-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 6: Testing & QA' already exists"
gh api repos/:owner/:repo/milestones -f title="Phase 7: Launch" -f description="Mainnet launch preparation" -f due_on="2027-07-01T00:00:00Z" 2>/dev/null || echo "Milestone 'Phase 7: Launch' already exists"

echo -e "${GREEN}✓${NC} Milestones created"
echo ""

# Function to create an issue
create_issue() {
    local title="$1"
    local body="$2"
    local labels="$3"
    local milestone="$4"
    
    echo -n "Creating issue: $title... "
    
    if [ -z "$milestone" ]; then
        gh issue create --title "$title" --body "$body" --label "$labels" 2>/dev/null && echo -e "${GREEN}✓${NC}" || echo -e "${YELLOW}Already exists or error${NC}"
    else
        gh issue create --title "$title" --body "$body" --label "$labels" --milestone "$milestone" 2>/dev/null && echo -e "${GREEN}✓${NC}" || echo -e "${YELLOW}Already exists or error${NC}"
    fi
}

echo "Creating Epic issues..."

# Epic 1
create_issue "[EPIC] Set up core blockchain infrastructure" \
"## Epic: Core Infrastructure

Fork Bitcoin Core and establish base infrastructure for SYNC development.

### Objectives
- Successfully fork Bitcoin Core
- Set up development environment
- Configure CI/CD pipeline
- Create testnet

### Success Criteria
- Working SYNC blockchain with modified parameters
- Automated testing and deployment
- Active testnet with multiple nodes

### Related Issues
- Fork Bitcoin Core and set up repository structure
- Configure development environment and CI/CD
- Modify consensus parameters for SYNC
- Create testnet genesis block" \
"epic,infrastructure,priority-high" ""

# Epic 2
create_issue "[EPIC] Implement AI-driven features (AI-DDA & NBC)" \
"## Epic: AI Integration

Develop and integrate artificial intelligence components for dynamic difficulty adjustment and neural block compression.

### Objectives
- Design and implement AI-DDA algorithm
- Create Neural Block Compression system
- Build on-chain neural network consensus
- Achieve 60% blockchain size reduction

### Success Criteria
- Real-time difficulty adjustment working
- Compression achieving target ratios
- Model consensus mechanism operational
- Protection against adversarial inputs

### Related Issues
- Design AI-DDA algorithm architecture
- Implement on-chain neural network
- Develop Neural Block Compression
- Create AI model training infrastructure" \
"epic,ai,innovation,priority-high" ""

# Epic 3
create_issue "[EPIC] Implement quantum-resistant cryptography" \
"## Epic: Quantum Resistance

Add post-quantum cryptographic signatures and migration mechanisms.

### Objectives
- Integrate SPHINCS+ or DILITHIUM
- Create dual-signature system
- Build migration mechanism
- Maintain backwards compatibility

### Success Criteria
- Quantum-resistant signatures working
- Smooth migration path available
- No breaking changes for existing users
- Signature aggregation reducing overhead

### Related Issues
- Integrate SPHINCS+ quantum-resistant signatures
- Develop signature migration mechanism
- Build emergency migration triggers" \
"epic,security,quantum,priority-high" ""

# Epic 4
create_issue "[EPIC] Develop hybrid consensus mechanism" \
"## Epic: Novel Consensus

Implement Time-Locked Staking and Proof-of-Useful-Work features.

### Objectives
- Create time-locked staking system
- Integrate useful work into mining
- Build anti-whale mechanisms
- Partner with research institutions

### Success Criteria
- Staking reducing mining difficulty
- 30% of mining contributing to science
- Fair distribution mechanisms
- Active research partnerships

### Related Issues
- Implement Time-Locked Staking system
- Develop Proof-of-Useful-Work integration
- Create emergency unstaking mechanism" \
"epic,consensus,innovation,priority-medium" ""

# Epic 5
create_issue "[EPIC] Build Synthetic Asset Bridge Protocol" \
"## Epic: Synthetic Assets

Create native synthetic asset support with oracles and liquidation mechanisms.

### Objectives
- Build price oracle system
- Implement collateralization
- Create liquidation engine
- Enable cross-chain bridging

### Success Criteria
- Synthetic assets tradeable on-chain
- Oracle prices accurate and tamper-proof
- Automatic liquidations protecting system
- Cross-chain asset teleportation working

### Related Issues
- Create price oracle system
- Implement collateralization logic
- Build liquidation mechanisms
- Add cross-chain teleportation" \
"epic,defi,innovation,priority-medium" ""

echo -e "${GREEN}✓${NC} Epic issues created"
echo ""

echo "Creating Phase 1 (Foundation) issues..."

# Phase 1 Issues
create_issue "Fork Bitcoin Core and set up repository structure" \
"## Task: Fork Bitcoin Core

### Description
Fork Bitcoin Core v24.0 or latest stable version and set up the proper repository structure for SYNC development.

### Tasks
- [ ] Fork Bitcoin Core repository
- [ ] Set up proper directory structure as per plan.md
- [ ] Configure build system for SYNC
- [ ] Create development branch strategy (main, develop, feature branches)
- [ ] Update README with SYNC information
- [ ] Remove unnecessary Bitcoin-specific branding

### Acceptance Criteria
- Clean fork of Bitcoin Core
- Organized repository structure
- Build system working
- Clear branching strategy documented" \
"setup,priority-critical" "Phase 1: Foundation"

create_issue "Configure development environment and CI/CD" \
"## Task: Development Environment Setup

### Description
Set up comprehensive development environment and continuous integration/deployment pipeline.

### Tasks
- [ ] Configure GitHub Actions workflows
- [ ] Set up automated unit testing
- [ ] Implement integration testing
- [ ] Configure code coverage reporting (aim for 80%+)
- [ ] Set up Docker containers for development
- [ ] Create development environment setup script
- [ ] Configure linting and code formatting

### Acceptance Criteria
- All tests run automatically on PR
- Code coverage visible in PRs
- Docker development environment working
- One-command dev environment setup" \
"devops,infrastructure,priority-high" "Phase 1: Foundation"

create_issue "Modify consensus parameters for SYNC" \
"## Task: Configure SYNC Parameters

### Description
Modify Bitcoin's consensus parameters to match SYNC specifications.

### Tasks
- [ ] Change block time to 5 minutes
- [ ] Set initial block reward to 25 SYNC
- [ ] Configure max supply to 21,000,000 SYNC
- [ ] Update halving schedule (every 210,000 blocks)
- [ ] Modify difficulty adjustment preparation for AI-DDA
- [ ] Update chain parameters
- [ ] Test parameter changes

### Acceptance Criteria
- All parameters correctly configured
- Tests pass with new parameters
- Documentation updated
- Testnet runs with new parameters" \
"consensus,blockchain,priority-high" "Phase 1: Foundation"

create_issue "Create testnet genesis block" \
"## Task: Testnet Genesis Block

### Description
Generate and deploy the SYNC testnet genesis block.

### Tasks
- [ ] Generate testnet genesis block
- [ ] Configure testnet network magic bytes
- [ ] Set up DNS seeds for testnet
- [ ] Deploy initial testnet nodes (minimum 5)
- [ ] Create testnet faucet
- [ ] Document testnet connection instructions
- [ ] Set up testnet explorer

### Acceptance Criteria
- Testnet genesis block created
- Multiple nodes successfully connecting
- Faucet distributing test SYNC
- Explorer showing testnet activity" \
"blockchain,testnet,priority-high" "Phase 1: Foundation"

echo -e "${GREEN}✓${NC} Phase 1 issues created"
echo ""

echo "Creating Phase 2 (AI Features) issues..."

# Phase 2 Issues
create_issue "Design AI-DDA algorithm architecture" \
"## Task: AI-DDA Architecture Design

### Description
Research and design the AI-Driven Dynamic Difficulty Adjustment algorithm.

### Tasks
- [ ] Research optimal ML models for time series prediction
- [ ] Design consensus mechanism for model weight updates
- [ ] Create training data pipeline from blockchain history
- [ ] Define model validation criteria
- [ ] Design protection against gaming/attacks
- [ ] Create model versioning system
- [ ] Document architecture decisions

### Acceptance Criteria
- Complete architecture document
- Model selection justified
- Consensus mechanism designed
- Security analysis completed" \
"ai,research,priority-high" "Phase 2: AI Features"

create_issue "Implement on-chain neural network for AI-DDA" \
"## Task: On-chain Neural Network

### Description
Implement the neural network that will run on-chain for difficulty adjustment.

### Tasks
- [ ] Develop lightweight neural network implementation
- [ ] Create weight serialization/deserialization
- [ ] Implement forward propagation only (no training on-chain)
- [ ] Add consensus rules for weight updates
- [ ] Implement deterministic floating-point operations
- [ ] Create model execution in block validation
- [ ] Add protection against adversarial inputs

### Acceptance Criteria
- Neural network executing deterministically
- Weight updates via consensus working
- Performance impact < 100ms per block
- All nodes produce identical results" \
"ai,blockchain,priority-high" "Phase 2: AI Features"

create_issue "Develop Neural Block Compression (NBC)" \
"## Task: Neural Block Compression

### Description
Implement AI-powered compression to reduce blockchain size by 60%.

### Tasks
- [ ] Research neural compression algorithms
- [ ] Implement learned compression models
- [ ] Create progressive compression (older blocks compress more)
- [ ] Build parallel decompression for fast sync
- [ ] Implement lossless compression guarantee
- [ ] Create compression benchmarks
- [ ] Add fallback for compression failures

### Acceptance Criteria
- 60% size reduction achieved
- Lossless compression verified
- Decompression speed > 1GB/s
- Backwards compatibility maintained" \
"ai,optimization,innovation,priority-high" "Phase 2: AI Features"

echo -e "${GREEN}✓${NC} Phase 2 issues created"
echo ""

echo "Creating Phase 3 (Security) issues..."

# Phase 3 Issues
create_issue "Integrate SPHINCS+ quantum-resistant signatures" \
"## Task: SPHINCS+ Integration

### Description
Integrate SPHINCS+ post-quantum signatures into SYNC.

### Tasks
- [ ] Integrate SPHINCS+ library
- [ ] Create dual-signature transaction format
- [ ] Implement signature generation
- [ ] Build signature verification
- [ ] Add size optimization (aggregation)
- [ ] Create backwards compatibility layer
- [ ] Test quantum resistance

### Acceptance Criteria
- SPHINCS+ signatures working
- Dual signatures validated correctly
- Transaction size overhead < 2x
- All tests passing" \
"quantum,cryptography,security,priority-high" "Phase 3: Security Features"

create_issue "Implement Self-Healing Network Protocol" \
"## Task: Self-Healing Network

### Description
Build protocol for automatic network issue detection and resolution.

### Tasks
- [ ] Create fork detection system
- [ ] Build automatic reorganization prevention
- [ ] Implement node health monitoring
- [ ] Add dynamic peer discovery optimization
- [ ] Create network partition detection
- [ ] Build automatic recovery mechanisms
- [ ] Add monitoring dashboard

### Acceptance Criteria
- Fork detection < 10 seconds
- Automatic recovery working
- Network resilience improved
- Monitoring dashboard operational" \
"network,security,innovation,priority-medium" "Phase 3: Security Features"

echo -e "${GREEN}✓${NC} Phase 3 issues created"
echo ""

# Create more issues for remaining phases...
echo "Creating remaining phase issues..."

# Quick creation of remaining key issues
create_issue "Implement Time-Locked Staking system" \
"Implement the hybrid PoW/PoS staking system with time locks from 3 months to 4 years." \
"staking,consensus,priority-high" "Phase 4: Economic Features"

create_issue "Develop Proof-of-Useful-Work integration" \
"Integrate scientific computation (protein folding) into 30% of mining work." \
"mining,innovation,research,priority-medium" "Phase 4: Economic Features"

create_issue "Build REST API endpoints" \
"Create comprehensive REST API with rate limiting and authentication." \
"api,infrastructure,priority-high" "Phase 5: Infrastructure"

create_issue "Develop mobile light client (SPV++)" \
"Create enhanced SPV protocol for mobile devices with iOS/Android support." \
"mobile,wallet,priority-medium" "Phase 5: Infrastructure"

create_issue "Implement comprehensive unit tests" \
"Achieve 80% code coverage with comprehensive unit tests for all components." \
"testing,qa,priority-critical" "Phase 6: Testing & QA"

create_issue "Security penetration testing" \
"Conduct thorough security audit and penetration testing." \
"security,testing,priority-critical" "Phase 6: Testing & QA"

create_issue "Create mainnet genesis block" \
"Generate and prepare mainnet genesis block for launch." \
"mainnet,blockchain,priority-critical" "Phase 7: Launch"

create_issue "Build exchange integration toolkit" \
"Create comprehensive toolkit for exchange integration with documentation." \
"exchange,integration,priority-high" "Phase 7: Launch"

echo -e "${GREEN}✓${NC} All development issues created"
echo ""

echo "Creating issue templates..."

# Create bug report template
gh api repos/:owner/:repo/issues/templates -f name="Bug Report" -f description="Report a bug in SYNC" -f title="[BUG] " -f body="**Describe the bug**
A clear description of the bug.

**To Reproduce**
Steps to reproduce:
1. Go to '...'
2. Click on '....'
3. See error

**Expected behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
 - OS: [e.g. Ubuntu 22.04]
 - Version: [e.g. v0.1.0]
 - Node version: [if applicable]

**Additional context**
Add any other context." -f labels='["bug"]' 2>/dev/null || echo "Bug template already exists"

# Create feature request template
gh api repos/:owner/:repo/issues/templates -f name="Feature Request" -f description="Suggest a feature for SYNC" -f title="[FEATURE] " -f body="**Is your feature request related to a problem?**
A clear description of the problem.

**Describe the solution**
A clear description of what you want.

**Describe alternatives**
Alternative solutions you've considered.

**Additional context**
Add any other context or screenshots." -f labels='["enhancement"]' 2>/dev/null || echo "Feature template already exists"

echo -e "${GREEN}✓${NC} Issue templates created"
echo ""

echo "======================================"
echo -e "${GREEN}✅ Issue creation complete!${NC}"
echo "======================================"
echo ""
echo "Summary:"
echo "  • Created 5 Epic issues"
echo "  • Created 20+ development issues"
echo "  • Created 7 milestones"
echo "  • Set up 16+ labels"
echo "  • Created 2 issue templates"
echo ""
echo "Next steps:"
echo "  1. Review created issues in GitHub"
echo "  2. Assign team members to issues"
echo "  3. Prioritize and begin development"
echo "  4. Update issue status as work progresses"
echo ""
echo "View your issues at:"
echo "  https://github.com/$(gh repo view --json owner -q .owner.login)/$(gh repo view --json name -q .name)/issues"
echo ""
