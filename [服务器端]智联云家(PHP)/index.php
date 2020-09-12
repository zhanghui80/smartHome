<?php
use Workerman\Worker;

/*workman框架*/
require_once __DIR__ . '/Workerman/Autoloader.php';
/*globaldata组件*/
require_once __DIR__ . '/GlobalData/src/Client.php';
require_once __DIR__ . '/GlobalData/src/Server.php';

// GlobalData Server
$global_worker = new GlobalData\Server('0.0.0.0', 2207);

// 创建一个Worker监听2345端口，使用http协议通讯
$http_worker = new Worker("http://0.0.0.0:7008");
// 启动4个进程对外提供服务
$http_worker->count = 4;

$http_worker->onWorkerStart = function($worker){
    global $global;
    $global = new GlobalData\Client('127.0.0.1:2207');
    //这里设置几个变量
};

// 接收到浏览器发送的数据时回复hello world给浏览器
$http_worker->onMessage = function($connection, $data)
{
    // 向浏览器发送hello world
    global $global;
    $option=$data['post']['data'];
    $secert=$data['post']['secret'];
    if($secert==12345678){
        switch ($option){
            case "open_air":
                $global->air="1";
                $global->tmp="20";
                echo '开空调'."\n";
                break;
            case "close_air":
                $global->air="0";
                echo '关空调'."\n";
                break;
            case "getdata":
                $datas['window']=$global->curtain;
                $datas['right']=$global->right;
                $datas['air_statue']= $global->air;
                $datas['air_temple']=$global->tmp;
                $datas['water']=$global->drink;
                $connection->send(json_encode($datas));
                return;
                break;
            case "open_drinkingfountain":
                echo '开饮水机'."\n";
                $global->drink="1";
                break;
            case "close_drinkingfountain":
                echo '关饮水机'."\n";
                $global->drink="0";
                break;
            case "open_right":
                echo '开灯'."\n";
                $global->right="1";
                break;
            case "close_right":
                echo '关灯'."\n";
                $global->right="0";
                break;
            case "open_curtain":
                echo '开窗帘'."\n";
                $global->curtain="0";
                break;
            case "close_curtain":
                echo '关窗帘'."\n";
                $global->curtain="1";
                break;
            default:
                //这里是温度操作
                if(substr($option,0,4)=='temp'){
                    $global->air="1";
                    $global->tmp=substr($option,5);
                    echo '空调温度'.$global->tmp."\n";
                }
        }
        $connection->send('ok');
        $global->issend=1;
    }else{
        $connection->send('error');
    }
    //$connection->send(json_encode($data));
};

// 创建一个Worker监听2347端口，不使用任何应用层协议
$tcp_worker = new Worker("tcp://0.0.0.0:7009");
// 启动4个进程对外提供服务
$tcp_worker->onWorkerStart = function($worker){
    global $global;
    $global = new GlobalData\Client('127.0.0.1:2207');
    //这里设置几个变量
    //是否发送
    $global->issend=0;
    //接收的数据
    $global->air="0";
    $global->drink="0";
    $global->right="0";
    $global->tmp="20";
    $global->curtain="0";
    $global->temp="";
    $global->temp2="";
};
$tcp_worker->count = 4;
// 当客户端发来数据时
$tcp_worker->onConnect = function($connection)
{
    echo "有新数据来自 ip " . $connection->getRemoteIp() . "\n";
};

$tcp_worker->onMessage = function($connection, $data)
{
    global $global;
    // 向客户端发送hello $data
    if($global->issend) {
        $global->issend = 0;
        echo '服务器发送'.'Maker116114' . $global->right . $global->curtain . $global->air . $global->tmp . $global->drink."\n";
        $connection->send('Maker116114' . $global->right . $global->curtain . $global->air . $global->tmp . $global->drink."b");
    }else {
        //获取数据
        $global->right=substr($data,11,1);
        $global->curtain=substr($data,12,1);
        $global->air=substr($data,13,1);
        $global->tmp=substr($data,14,2);
        $global->drink=substr($data,16,1);
        if($global->temp!=$data){
            $global->temp=$data;
            echo "发送的数据".$data."\n";
            echo "拼凑的数据".'Maker116114' . $global->right . $global->curtain . $global->air . $global->tmp . $global->drink."\n";
        }
        //$connection->send('Maker116114' . $global->right . $global->curtain . $global->air . $global->tmp . $global->drink);
       // $connection->send('nodata');
    }
    //$connection->send('Maker116114');
};
// 运行worker
Worker::runAll();


