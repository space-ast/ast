import { defineUserConfig } from "vuepress";

import theme from "./theme.js";

export default defineUserConfig({
  base: "/",
  lang:"zh-CN",

  locales: {
    "/": {
      lang: "en-US",
      title: "ast docs",
      description: "docs for ast project",
    },
    "/zh/": {
      lang: "zh-CN",
      title: "ast文档",
      description: "ast 项目的文档主页",
    },
  },

  theme,

  // Enable it with pwa
  // shouldPrefetch: false,
});
