# SerialyTTY Documentation

Professional documentation for the SerialyTTY USB-TTL Bridge project.

## 📖 Documentation Structure

```
content/docs/
├── index.md                    # Main landing page
└── en/                         # English documentation
    ├── getting-started/        # Setup and installation
    ├── hardware/               # Pin configurations and wiring
    ├── features/               # Feature descriptions
    └── architecture/           # System design and modules
```

## 🚀 Quick Start

### Prerequisites

- Node.js 18+ and npm
- Git

### Install Dependencies

```bash
cd publishing/docusaurus
npm install
```

### Local Development

```bash
cd publishing/docusaurus
npm start
```

This starts a local development server at `http://localhost:3000` with live reload.

### Build for Production

```bash
cd publishing/docusaurus
npm run build
```

The static files will be generated in `publishing/docusaurus/build/`.

### Serve Production Build

```bash
cd publishing/docusaurus
npm run serve
```

## 📝 Writing Documentation

### Adding New Pages

1. Create a new markdown file in `content/docs/en/<section>/`
2. Add frontmatter:

```markdown
---
id: my-page
title: My Page Title
sidebar_position: 1
---

# Content here
```

3. Sync to publishing directory:

```bash
rsync -av content/docs/ publishing/docusaurus/docs/
```

### Frontmatter Options

| Field | Description | Required |
|-------|-------------|----------|
| `id` | Unique identifier | Yes |
| `title` | Page title | Yes |
| `sidebar_position` | Order in sidebar | No |
| `sidebar_label` | Custom sidebar text | No |
| `description` | Meta description | No |
| `tags` | Topic tags | No |

### Markdown Features

**Code blocks with syntax highlighting:**

```cpp
void setup() {
    Serial.begin(115200);
}
```

**Admonitions:**

:::note
This is a note
:::

:::tip
This is a tip
:::

:::warning
This is a warning
:::

:::danger
This is dangerous
:::

**Tables:**

| Feature | Status |
|---------|--------|
| UART Bridge | ✅ |
| Baud Detection | ✅ |

## 🎨 Customization

### Theme Configuration

Edit `publishing/docusaurus/docusaurus.config.ts`:

- Site title and tagline
- Logo and favicon
- Navbar items
- Footer content
- Color theme

### Custom CSS

Edit `publishing/docusaurus/src/css/custom.css` for:

- Custom colors
- Font changes
- Component styling
- Responsive breakpoints

## 🌐 Deployment

### GitHub Pages

1. Build the documentation:
```bash
cd publishing/docusaurus
npm run build
```

2. Deploy to GitHub Pages:
```bash
GIT_USER=<your-github-username> npm run deploy
```

Or use the Makefile:
```bash
make docs-deploy
```

### Custom Domain

Add a `CNAME` file to `publishing/docusaurus/static/`:

```
docs.serialytty.com
```

## 📋 Maintenance

### Syncing Content

After editing files in `content/docs/`, sync to publishing:

```bash
rsync -av content/docs/ publishing/docusaurus/docs/
```

### Version Control

- Source files: `content/docs/`
- Build artifacts: `publishing/docusaurus/build/` (git ignored)
- Dependencies: `publishing/docusaurus/node_modules/` (git ignored)

### Updating Dependencies

```bash
cd publishing/docusaurus
npm update
npm audit fix
```

## 🔍 Search

Docusaurus includes built-in search functionality. For advanced search:

1. Enable Algolia DocSearch
2. Add configuration to `docusaurus.config.ts`
3. Submit site to Algolia DocSearch program

## 📊 Analytics

Add Google Analytics or Plausible:

```typescript
// In docusaurus.config.ts
gtag: {
  trackingID: 'G-XXXXXXXXXX',
},
```

## 🤝 Contributing

1. Edit documentation in `content/docs/`
2. Sync to publishing directory
3. Test locally with `npm start`
4. Commit changes to git
5. Build and deploy

## 📄 License

Documentation is licensed under CC-BY-4.0. Code examples are MIT.

---

Built with [Docusaurus](https://docusaurus.io/) and ❤️
