//index.js
import tools from '../../utils/tools.js';
//获取应用实例
const app = getApp()
import Toast from '../../miniprogram_npm/@vant/weapp/toast/toast';
Page({
  data: {
    status: false,
    air: 16, // 空调温度
    temperature: 0, // 温度
    humidity: 0, // 湿度
    curtain: false, // 窗帘
    drink: false, // 饮水机当前的状态
    light: false, // 灯关状态
    swiperList: [{
      id: 0,
      type: 'image',
      url: 'https://img.xiaoyou66.com/images/2020/10/12/pt9e.png'
    }, {
      id: 1,
        type: 'image',
        url: 'https://img.xiaoyou66.com/images/2020/10/12/pyLt.png',
    }, {
      id: 2,
      type: 'image',
      url: 'https://img.xiaoyou66.com/images/2020/10/12/p2oc.jpg'
    }]
  },
  onLoad: function () {
    // 每两秒获取一次温度信息
    setInterval(()=>{
      this.getTemp();
    },2000)
  },
  getTemp(){ // 获取当前空调的温度
    tools.requests(app.globalData.SERVER + '/api/v1/smart/get/temp',null, 'get').then((res) => {
      if(res.code == 1){
        let data = res.data
        this.setData({
          temperature: data.temperature,
          humidity: data.humidity,
          air: data.air,
          status: data.air !== 0,
          curtain: data.curtain,
          drink:data.drink,
          light:data.light
        })
      }
    });
  },
  changeAirTemp(event){ // 设置当前空调温度
    let temp = event.detail;
    // 获取用户信息
    let data = this.getInfo()
    if (data == null){
      return
    }
    data.temp =  temp
    tools.requests(app.globalData.SERVER + '/api/v1/smart/set/temp',data, 'post').then((res) => {
      if(res.code == 1){
        this.setData({ air: temp });
      } else {
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  },
  onChange(){ // 空调开关
    let status = this.data.status
    // 获取用户信息
    let data = this.getInfo()
    if (data == null){
      return
    }
    // 判断当前状态
    if(!status){
      data.temp = 26
    } else {
      data.temp = 0
    }
    tools.requests(app.globalData.SERVER + '/api/v1/smart/set/temp',data, 'post').then((res) => {
      if(res.code == 1){
        this.setData({ status: !status })
      }else{
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  },
  // 点击开锁
  openLock: function() {
    // 提示信息
    const toast = Toast.loading({
      duration: 0,
      message: '正在开门',
      forbidClick: true,
    });
    // 获取用户信息
    let data = this.getInfo()
    if (data == null){
      return
    }
    // 发送开锁请求
    tools.requests(app.globalData.SERVER + '/api/v1/smart/open/door', data, 'get').then((res) => {
      if(res.code == 1){
        setTimeout(()=>{
          Toast.clear();
          wx.showToast({title: '开锁成功'})
        },10)
      } else {
        setTimeout(()=>{
          Toast.clear();
        wx.showToast({title: res.msg, icon: 'none'})
        },10)
      }
    })
  },
  openCurtain(){ // 打开窗帘
    let status = this.data.curtain
    let data = this.getInfo()
    if (data == null){
      return
    }
    data.open = status ? '0':'1'
    // 获取用户信息
    tools.requests(app.globalData.SERVER + '/api/v1/smart/change/curtain',data, 'post').then((res) => {
      if(res.code == 1){
        wx.showToast({title: '操作成功', icon: 'none'}) 
      } else {
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  },
  openDrink(){ // 打开饮水机
    let status = this.data.drink
    let data = this.getInfo()
    if (data == null){
      return
    }
    data.open = status ? '0':'1'
    // 获取用户信息
    tools.requests(app.globalData.SERVER + '/api/v1/smart/change/drink',data, 'post').then((res) => {
      if(res.code == 1){
        wx.showToast({title: '操作成功', icon: 'none'})
      } else {
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  },
  openLight(){ // 打开灯关
    let status = this.data.light
    let data = this.getInfo()
    if (data == null){
      return
    }
    data.open = status ? '0':'1'
    // 获取用户信息
    tools.requests(app.globalData.SERVER + '/api/v1/smart/change/light',data, 'post').then((res) => {
      if(res.code == 1){
        wx.showToast({title: '操作成功', icon: 'none'})
      } else {
        wx.showToast({title: res.msg, icon: 'none'})
      }
    })
  },
  getInfo(){ // 获取个人信息
    this.vibrate(45)
    let data = {}
    try {
      data = {
        openid: wx.getStorageSync('openid'),
      }
    } catch (e) {
      wx.showToast({title: '出现意外请重试', icon: 'none'})
      data == null
    }
    return data
  },
  vibrate(time){
    let count=time/15;
    let index=0;
    let interval=setInterval(function(){
      wx.vibrateShort();
      index++;
      if(index>count){
        clearTimeout(interval);
        interval=null;
      }
   
    },15);
  }

})
