// @Title  数据层
// @Description  这里主要放数据的返回结构体
package model

import (
	"github.com/labstack/echo/v4"
	"net/http"
)


// api返回的格式
type Data struct {
	Code int         `json:"code"` //返回码 1为获取数据成功 0为失败
	Msg  string      `json:"msg"`  // 返回的信息
	Data interface{} `json:"data"` // 返回的数据
}

// 返回正确的结果
func DisplayOk(c echo.Context, data interface{}, msg string) error {
	return c.JSON(http.StatusOK, Data{Code: 1, Data: data, Msg: msg})
}

// 返回错误的结果
func DisplayError(c echo.Context, msg string) error {
	return c.JSON(http.StatusOK, Data{Code: 0, Data: nil, Msg: msg})
}

// 空调和温度的数据
type SmartTemp struct {
	Temperature float64 `json:"temperature"` // 温度
	Humidity int	`json:"humidity"` // 湿度
	Air int	`json:"air"` // 空调当前的温度
	Curtain bool	`json:"curtain"` // 窗帘
	Drink bool	`json:"drink"` // 饮水机
	Light bool	`json:"light"` // 灯光
}

// 当前窗帘的状态
type SmartCurtain struct {
	Status bool	`json:"status"`
}