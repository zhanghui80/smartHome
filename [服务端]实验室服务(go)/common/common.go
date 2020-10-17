// @Title  通用函数
// @Description  比如一些通用的工具类
package common

import (
	"io/ioutil"
	"net/http"
	"strconv"
	"time"
)

//时间转string
func Time2String(time time.Time, showHour bool) string {
	if showHour {
		return time.Format("2006-01-02 15:04:05")
	}
	return time.Format("2006-01-02")
}

// 字符串转int
func String2Int(str string) int {
	if data, err := strconv.Atoi(str); err != nil {
		return 0
	} else {
		return data
	}
}


func HttpGet(url string) string {
	resp, err := http.Get(url)
	if err != nil {
		return ""
	}
	defer resp.Body.Close()
	s, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return ""
	}
	return string(s)
}