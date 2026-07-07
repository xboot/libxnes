/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
const sidebars = {
  guide: [
    {
      type: 'category',
      label: '入门指南',
      link: { type: 'doc', id: 'guide/intro' },
      items: [
        'guide/intro',
        'guide/build-guide',
        'guide/getting-started',
      ],
    },
  ],
  api: [
    {
      type: 'category',
      label: 'API 参考',
      link: { type: 'doc', id: 'api/architecture' },
      items: [
        'api/architecture',
        'api/context',
        'api/cartridge',
        'api/cpu',
        'api/ppu',
        'api/apu',
        'api/controller',
        'api/state',
      ],
    },
  ],
  examples: [
    'examples/basic-usage',
    'examples/audio',
    'examples/input',
    'examples/rewind',
  ],
  reference: [
    'reference/mappers',
    'reference/controls',
    'reference/links',
  ],
};

module.exports = sidebars;
