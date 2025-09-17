# Contributing to SyntheticCoin (SYNC)

Thank you for your interest in contributing to SyntheticCoin! This revolutionary cryptocurrency project aims to integrate AI, quantum resistance, and scientific computing into blockchain technology.

## Getting Started

### Prerequisites
- C++ development experience
- Familiarity with Bitcoin Core codebase (helpful but not required)
- Git and GitHub knowledge
- Linux/Unix development environment

### Setting Up Development Environment

1. **Clone the repository:**
   ```bash
   git clone https://github.com/ymode/SyntheticCoin-SYNC.git
   cd SyntheticCoin-SYNC
   ```

2. **Check available issues:**
   ```bash
   gh issue list --label "good first issue"
   ```

3. **Pick an issue and assign yourself:**
   ```bash
   gh issue edit <issue-number> --add-assignee @me
   ```

## Development Workflow

1. **Create a feature branch:**
   ```bash
   git checkout -b feature/issue-<number>-description
   ```

2. **Make your changes following our coding standards**

3. **Test your changes:**
   ```bash
   make check
   ```

4. **Commit with descriptive message:**
   ```bash
   git commit -m "feat: Add feature X for issue #<number>

   - Detailed change 1
   - Detailed change 2"
   ```

5. **Push and create PR:**
   ```bash
   git push origin feature/issue-<number>-description
   gh pr create --fill
   ```

## Coding Standards

### C++ Style
- Follow Bitcoin Core coding conventions
- Use 4 spaces for indentation (no tabs)
- Maximum line length: 100 characters
- Use descriptive variable names
- Add comments for complex logic

### Commit Messages
We follow conventional commits:
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `test:` Test additions/changes
- `refactor:` Code refactoring
- `perf:` Performance improvements
- `chore:` Maintenance tasks

### Testing Requirements
- All new features must include unit tests
- Maintain or increase code coverage (target: 80%)
- Integration tests for consensus changes
- Performance benchmarks for optimizations

## Priority Areas

### High Priority (Help Needed!)
1. **AI-DDA Implementation** - Neural network for difficulty adjustment
2. **Quantum Signatures** - SPHINCS+ integration
3. **Block Compression** - Neural compression algorithms
4. **Testnet Setup** - Initial network deployment

### Good First Issues
- Documentation improvements
- Unit test additions
- Build system improvements
- Code cleanup and refactoring

## Communication

### Discord Channels
- `#dev-general` - General development discussion
- `#dev-ai` - AI feature development
- `#dev-quantum` - Quantum resistance features
- `#dev-consensus` - Consensus mechanism changes

### Weekly Meetings
- Developer sync: Thursdays 3pm UTC
- Community call: Saturdays 6pm UTC

## Review Process

1. All PRs require at least 2 reviews
2. CI must pass (tests, linting, coverage)
3. Documentation must be updated
4. Performance impact assessed
5. Security implications reviewed

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Eligible for bug bounties
- Considered for core team positions
- Invited to exclusive events
- Potentially receive SYNC tokens (post-launch)

## Resources

### Documentation
- [Development Plan](plan.md) - Project roadmap
- [GitHub Issues](https://github.com/ymode/SyntheticCoin-SYNC/issues) - Current tasks
- [Bitcoin Developer Guide](https://developer.bitcoin.org/) - Background knowledge
- [AI/ML Resources](docs/ai-resources.md) - AI implementation guides

### Tools
- GitHub CLI: `gh` for issue/PR management
- Bitcoin Core docs: Reference implementation
- SPHINCS+ library: Quantum signatures
- TensorFlow Lite: Neural network inference

## Questions?

- Open a discussion: `gh discussion create`
- Ask in Discord: `#dev-help`
- Email: dev@syntheticcoin.io

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Join us in building the future of cryptocurrency! 🚀**

*"Synthesizing innovation, one commit at a time."*
