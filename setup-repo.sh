#!/bin/bash

# SyntheticCoin SYNC - Repository Setup Script
# This script helps set up the GitHub repository and authentication

set -e

echo "======================================"
echo "SyntheticCoin SYNC - Repository Setup"
echo "======================================"
echo ""

# Add ~/.local/bin to PATH if needed
export PATH="$HOME/.local/bin:$PATH"

# Check if gh is installed
if ! command -v gh &> /dev/null; then
    echo "❌ GitHub CLI not found. Please install it first."
    exit 1
fi

echo "✅ GitHub CLI found: $(gh --version | head -1)"
echo ""

# Check authentication status
if ! gh auth status >/dev/null 2>&1; then
    echo "📝 You need to authenticate with GitHub."
    echo ""
    echo "Please run the following command to authenticate:"
    echo ""
    echo "  gh auth login"
    echo ""
    echo "Follow these steps:"
    echo "  1. Choose 'GitHub.com'"
    echo "  2. Choose 'HTTPS' for protocol"
    echo "  3. Authenticate with browser or token"
    echo "  4. For browser: press Enter and follow the prompts"
    echo "  5. For token: create one at https://github.com/settings/tokens"
    echo ""
    echo "After authentication, run this script again."
    exit 0
fi

echo "✅ Authenticated with GitHub as: $(gh api user -q .login)"
echo ""

# Check if remote repository exists
if gh repo view >/dev/null 2>&1; then
    echo "✅ Repository already exists on GitHub"
    REPO_URL=$(gh repo view --json url -q .url)
    echo "   URL: $REPO_URL"
else
    echo "📦 Creating GitHub repository..."
    read -p "Do you want to create the repository now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        # Create the repository
        gh repo create SyntheticCoin-SYNC \
            --public \
            --description "SyntheticCoin (SYNC) - A next-generation SHA-256 cryptocurrency with AI-driven consensus and quantum-resistant features" \
            --homepage "https://syntheticcoin.io" \
            --confirm
        
        echo "✅ Repository created successfully!"
    else
        echo "Skipping repository creation."
    fi
fi

echo ""
echo "📋 Next steps:"
echo ""
echo "1. Make your first commit:"
echo "   git add ."
echo "   git commit -m \"Initial commit: Project foundation and planning\""
echo ""
echo "2. Push to GitHub:"
echo "   git branch -M main"
echo "   git push -u origin main"
echo ""
echo "3. Create GitHub issues for development:"
echo "   ./create-github-issues.sh"
echo ""
echo "4. View your repository:"
echo "   gh repo view --web"
echo ""
echo "======================================"
echo "Ready to build the future of cryptocurrency! 🚀"
echo "======================================"
