import { defineUserConfig } from "vuepress";
import { getDirname, path } from 'vuepress/utils'
import theme from "./theme.js";

const __dirname = getDirname(import.meta.url)

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
  markdown:{
    importCode:{
      handleImportPath:  (str) => 
        str
        .replace(/^\/examples/, path.resolve(__dirname, '../../examples'))
        .replace(/^\/src/, path.resolve(__dirname, '../../src'))
      ,
    }
  }

  // Enable it with pwa
  // shouldPrefetch: false,
});
