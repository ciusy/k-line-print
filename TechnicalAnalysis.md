1.  客户端

  * 本地应用
    * Windows7 (图形DUILIB 网络POCO库支持Websocket,待验证)

  * HTML5浏览器
    * 首选支持Chrome浏览器，远期支持所有HTML5浏览器
    * 图形库：KineticJS

  * 本地应用(远期)
    * IOS(SocketRocket)
    * Android
    * 各平台统一使用WebSocket

2 .  服务器

  * HTTP 服务器
  * NodeJS + Socket.IO，WebSocket协议(首选)
  * 数据库 MySQL

Socket.IO在WebSocket之上封装，不能直接适配POCO的Websocket