import type {Config} from '@docusaurus/types';
import {themes as prismThemes} from 'prism-react-renderer';

const {
  RELEASE_TAG = 'v1.0',
  COMMIT_SHA = 'local',
  BUILD_DATE = '',
  GITHUB_REPO_URL = 'https://github.com/thenisvan/SerialyTTY',
  SITE_URL = 'https://thenisvan.github.io',
  BASE_URL = '/SerialyTTY/',
} = process.env;

const commitLink =
  GITHUB_REPO_URL && COMMIT_SHA ? `${GITHUB_REPO_URL}/commit/${COMMIT_SHA}` : '';

const config: Config = {
  title: 'SerialyTTY Documentation',
  url: SITE_URL,
  baseUrl: BASE_URL,
  deploymentBranch: 'gh-pages',
  favicon: 'img/favicon.ico',

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          path: 'docs',
          routeBasePath: '/',
          sidebarPath: require.resolve('./sidebars-serialytty.ts'),
          includeCurrentVersion: true,
          editCurrentVersion: false,
        },
        // Blog nepoužívame – vypneme, aby nevznikal duplicitný /tags
        blog: false,
        theme: { customCss: require.resolve('./src/css/custom.css') },
      },
    ],
  ],

  // (Optional) Plugins — buildInfoPlugin disabled (file not present)
  plugins: [],

  themeConfig: {
    docs: {
      sidebar: {
        hideable: true,
        autoCollapseCategories: false,
      },
    },
    navbar: {
      title: 'SerialyTTY',
      logo: {
        alt: 'SerialyTTY',
        src: 'img/logo.png',
        srcDark: 'img/logo-dark.png',
        target: '_self',
        href: '/',
      },
      items: [
        { to: '/en/getting-started', label: 'Getting Started', position: 'left' },
        { to: '/en/hardware', label: 'Hardware', position: 'left' },
        { to: '/en/features', label: 'Features', position: 'left' },
        {
          href: GITHUB_REPO_URL || 'https://github.com/thenisvan/SerialyTTY',
          label: 'GitHub',
          position: 'right',
        },
        {
          href: commitLink || '#',
          label: `${RELEASE_TAG} • ${COMMIT_SHA}`,
          position: 'right',
        },
      ],
    },

    // Announcement bar hore (možno kedykoľvek vypnúť/komentovať)
   // announcementBar: {
    //  id: 'build_info',
    //  content: commitLink
    //    ? `🔖 <strong>Release:</strong> ${RELEASE_TAG} &nbsp;•&nbsp; <strong>Commit:</strong> <a href="${commitLink}" target="_blank" rel="noopener noreferrer">${COMMIT_SHA}</a> &nbsp;•&nbsp; <strong>Build:</strong> ${BUILD_DATE}`
    //    : `🔖 <strong>Release:</strong> ${RELEASE_TAG} &nbsp;•&nbsp; <strong>Commit:</strong> ${COMMIT_SHA} &nbsp;•&nbsp; <strong>Build:</strong> ${BUILD_DATE}`,
   //   backgroundColor: '#f7f70aff',
    //  textColor: '#091E42',
   //   isCloseable: true,
   // },

    footer: {
      style: 'dark',
      copyright: `
  <div style="text-align:center;">
    © ${new Date().getFullYear()} SerialyTTY Project<br/>
    🔖 Version: <strong>${RELEASE_TAG}</strong><br/>
    💡 Commit: <code>${COMMIT_SHA}</code><br/>
    🕒 Build: ${BUILD_DATE}<br/>
    Built with ESP32-C6 and ❤️
  </div>
`,
    },

    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
      additionalLanguages: ['bash', 'cpp', 'ini', 'makefile'],
    },
  },

  future: {
    experimental_faster: false, // disables experimental minifier causing build crashes
  },
};

export default config;