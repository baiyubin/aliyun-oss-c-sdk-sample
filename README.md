#Aliyun OSS C SDK Sample 

## 关于
 - aliyun-oss-c-sdk-sample是aliyun-oss-c-sdk的基于Vistual Studio的示例工程。
 - 示例工程包括Vistual Studio 2008(V9.0)/2010(V10.0)/2012(V11.0)/2013(V12.0)/2015(V14.0)的WIN32(x86)工程。
 
## 版本
 - 当前版本：0.2.0

## 环境
 - Win7及以上
 
## 运行
 - 使用Vistual Studio打开对应版本的示例工程，即可直接编译运行。

> 注意：
> - oss-c-sdk-sample只支持WIN32(x86)，不支持x64。
> - 编译oss-c-sdk-sample前，请修改Endpoint/AccessKeyID/AccessKeySecret/BucketName为正确配置。
> - 运行oss-c-sdk-sample前，请做如下配置。在Sulution Explore中选择工程aliyun-oss-c-sdk，右击选择Property，在oss-c-sdk-sample Property Pages中配置Configuration Properties -> Debugging -> Environment为PATH=..\oss-c-sdk\lib\Release\;%PATH%。

## 作者
- Yubin Bai

## License
- MIT
