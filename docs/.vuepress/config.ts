import { defineUserConfig } from "vuepress";

import theme from "./theme.js";

export default defineUserConfig({
  base: "/ast/",
  lang:"zh-CN",

  locales: {
    "/en/": {
      lang: "en-US",
      title: "ast docs",
      description: "docs for ast project",
    },
    "/": {
      lang: "zh-CN",
      title: "ast文档",
      description: "ast 项目的文档主页",
    },
  },

  theme,

  // Enable it with pwa
  // shouldPrefetch: false,
});
