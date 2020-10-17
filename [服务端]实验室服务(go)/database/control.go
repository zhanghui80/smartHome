// @Title  数据库控制层
// @Description  这里封装了数据库的请求，用于后面函数调用
package database

import (
	"fmt"
	"gorm.io/driver/mysql"
	"gorm.io/gorm"
)

var Db *gorm.DB

// 数据库初始化
func DbInit() {
	var err error
	// 开始连接数据库
	dsn := "maker:123456@tcp(127.0.0.1:3306)/maker?charset=utf8mb4&parseTime=True&loc=Local"
	if Db, err = gorm.Open(mysql.Open(dsn), &gorm.Config{}); err != nil {
		fmt.Println("出现错误")
		panic("failed to connect database")
	}
}

// 创建表
func CreateTable(data interface{}) error {
	return Db.AutoMigrate(data)
}

// 插入数据
func InsertData(data interface{}) error {
	return Db.Create(data).Error
}

// 读取最后一条数据
func TakeLast(data interface{}) error {
	return Db.Last(data).Error
}

// 查找数据
func FindFirstData(data interface{},key string,value string) error {
	return Db.Where(key + " = ?", value).First(data).Error
}