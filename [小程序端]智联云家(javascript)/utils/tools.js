const app = getApp()

const requests=(url,data,type)=>{
  const errData = {
    code: 0,
    data: null,
    msg: '请求发送失败'
  }
  return new Promise((resove,reject)=>{
      return wx.request({
        url: url,
        method: type,
        data: data,
        header: {
          "content-type": "application/x-www-form-urlencoded"
        },
        success(response) {
          resove(response.data)
        },
        fail(err){
          //请求错误 也返回正确信息
          resove(errData)
          //reject(err.data)
          //如果使用reject就是需要使用catch才能获取数据
        }
      })
  })
}


// 封装函数,外部可以引用
module.exports={
  requests
}