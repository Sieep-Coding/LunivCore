// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// https://astro.build/config
export default defineConfig({
	integrations: [
		starlight({
			title: 'LunivCore',
			social: [
				{ icon: 'github', label: 'GitHub', href: 'https://github.com/sieep-coding/lunivcore' }

			],
			sidebar: [
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
			],
		}),
	],
});
