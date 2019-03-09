
namespace cl tutorial
namespace cpp tutorial


struct Item {
  1: i32 key
  2: string value
}

service Service {
  Item getStruct(1: i32 key)
}


service QueryService extends Service {

   void ping(),

   i32 add(1:i32 num1, 2:i32 num2),

   i32 q(1:i32 logid, 2:i64 w) 

}
