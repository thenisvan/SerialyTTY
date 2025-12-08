import type { SidebarsConfig } from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  docsSidebar: [
    {
      type: 'doc',
      id: 'SERIALYTTY_HOME',
      label: '🏠 Home',
    },
    {
      type: 'category',
      label: '🚀 Getting Started',
      collapsed: false,
      link: {
        type: 'doc',
        id: 'en/getting-started/index',
      },
      items: [],
    },
    {
      type: 'category',
      label: '🔧 Hardware',
      link: {
        type: 'doc',
        id: 'en/hardware/index',
      },
      items: [],
    },
    {
      type: 'category',
      label: '⚡ Features',
      link: {
        type: 'doc',
        id: 'en/features/index',
      },
      items: [],
    },
    {
      type: 'category',
      label: '💻 Architecture',
      link: {
        type: 'doc',
        id: 'en/architecture/index',
      },
      items: [],
    },
  ],
};

export default sidebars;
