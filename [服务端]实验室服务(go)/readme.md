# Go实现

## 创建人：小游

## 说明：

本demo api服务采用echo框架([github地址](https://github.com/labstack/echo))，tcp请求采用自带的net库，数据库采用gorm框架([github地址](https://github.com/go-gorm/gorm)) [gorm中文文档](https://gorm.io/zh_CN/docs/models.html)

## 文件结构:

```
│  go.mod   // gomod文件
│  go.sum  // go的包校验文件
│  main.go // 主函数
│  readme.md // readme makerdown文件
│
├─.idea  // idea配置文件，不用管
│      go.iml
│      modules.xml
│      vcs.xml
│      workspace.xml
│
├─api	// api服务器文件
│      api.go // api入口文件
│      model.go// api返回数据模型
│      router.go// api表示层
│      server.go// api逻辑层
│
├─common
│      common.go // 通用函数
│
├─database // 数据库操作文件
│      control.go // 数据库的控制层
│      model.go// 数据库的模型层，里面存放ORM持久化结构
│
└─tcp // TCP服务器文件
        tcp.go // tcp处理函数
```



## 注意事项:

在发送TCP数据的时候请多带一个数据`$ADD,48,0`，这个主要是为了避免数据转换的时候出现错误。