# ghcli

A lightweight GitHub CLI tool written in C to interact with pull requests of your current Git branch.

---

## ✅ Features

### `ghcli pr`

Checks for an open pull request on your current Git branch and returns:

- Branch name
- Pull request number
- PR title
- Number of ✅ approvals

Example output:

```
Branch: feature/improve-api | PR: #42 | Title: Refactor endpoint | Approvals: 3 ✅
```

---

## 🚀 How to Build

Make sure you have `libcurl` and `cJSON` installed (via Homebrew on macOS):

```bash
brew install curl cjson
```

Then build:

```bash
make
```

This will create a binary called `ghcli`.

---

## ⚙️ How to Use Anywhere

To use `ghcli` globally from any Git repo, move or symlink the binary to a folder in your `$PATH`.

### Option 1: Move it (requires sudo)

```bash
sudo mv ghcli /usr/local/bin/
```

### Option 2: Symlink it to a local bin folder

```bash
mkdir -p ~/bin
ln -s "$(pwd)/ghcli" ~/bin/ghcli
```

Then add `~/bin` to your shell config if it isn’t already:

```bash
# ~/.zshrc or ~/.bashrc
export PATH="$HOME/bin:$PATH"
```

Reload your shell:

```bash
source ~/.zshrc  # or ~/.bashrc
```

Now you can use `ghcli pr approvals` in any GitHub repo.

---

## 🔐 Authentication

You need a GitHub personal access token with `repo` access.  
Export it as an environment variable:

```bash
export GITHUB_TOKEN=your_token_here
```

---

## 📦 Planned Features

- `ghcli pr checks`: List status of all GitHub Actions / Jenkins / SonarCloud checks
- `ghcli pr mergeable`: Show mergeability status
- `ghcli issues list`: Show assigned GitHub issues

---

## 🧠 Project Structure

```
src/
├── main.c
├── pr/
│   ├── pr.c
│   └── pr.h
├── utils/
│   ├── utils.c
│   └── utils.h
```

---

## 👨‍💻 Contributing

Contributions welcome. Fork the repo, make a feature branch, and submit a PR.

---

## 📄 License

MIT
