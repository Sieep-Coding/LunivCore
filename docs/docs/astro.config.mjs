// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';
import react from '@astrojs/react';
import vercel from '@astrojs/vercel';

// https://astro.build/config
export default defineConfig({
	output: 'server',
    adapter: vercel(),
	integrations: [
		react(),
		starlight({
			title: 'LunivCore',
			customCss: [
				'./src/styles/custom.css',
			],
			social: [
				{ icon: 'github', label: 'GitHub', href: 'https://github.com/sieep-coding/lunivcore' }

			],
			sidebar: [
				{
					label: 'Application', // <<< NEW SECTION
                    items: [
                        { label: 'Analytics Builder', link: '/app' }, // <<< NEW LINK
                    ],
				},
				{
					label: 'Start Here',
					autogenerate: { directory: 'getting-started' },
				},
				{
					label: 'Guides',
					items: [
						{ label: 'Example Guide', slug: 'guides/example' },
					],
				},
				{
					label: 'Reference',
					autogenerate: { directory: 'reference' },
				},
				{
					label: 'Invest or Support',
					autogenerate: { directory: 'invest' },
				},
			],
		}),
	],
});
