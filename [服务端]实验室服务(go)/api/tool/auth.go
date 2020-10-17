// @Title  认证函数
// @Description  请填写文件描述
package tool

import (
	"github.com/labstack/echo/v4"
	"tcp/database"
)

// 用户权限认证
func AuthUser(openid string) bool {
	user:=new(database.UserInfo)
	err := database.FindFirstData(user,"openid",openid)
	if err!=nil && err.Error() == "record not found"{
		return false
	} else if err == nil {
		return true
	}
	return false
}

// 用户权限认证带返回值
func AuthReturn(c echo.Context) bool {
	openid:=c.FormValue("openid")
	if openid==""{
		return false
	}
	// 判断openid是否在数据库中
	if AuthUser(openid){
		return true
	}
	return false
}