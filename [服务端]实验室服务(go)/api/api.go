// @Title  api入口
// @Description  这里主要放api服务器初始化函数
package api

import (
	"github.com/labstack/echo/v4"
	"tcp/api/router"
)

// API服务器初始化
func WebInit() {
	// 初始化一个echo实例
	e := echo.New()
	// 开启路由
	router.SmartRouter(e)
	// 启动服务
	e.Logger.Fatal(e.Start(":2334"))
}
