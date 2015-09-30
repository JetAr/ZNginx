//z 2015-09-29 21:08:27 L.93 '10293 T3584140510.K
Gets the angle from the quaternion. You may think the following ways at first.
Calculates the pitch angle and the yaw angle, that a direction you give and another direction of the quaternion make. (Ignores Z-ROLL)
Gives two vectors that are the front direction and the up direction, makes a projection and calculates pitch, yaw and roll angle.

On the other hand, here is an expression to get angles from x, y, z and w of the quaternion:

float Quaternion::getPitch()
{
  return atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
}

float Quaternion::getYaw()
{
  return asin(-2*(x*z - w*y));
}

float Quaternion::getRoll()
{
  return atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
}


The angles gotten by this expression are the shortest possible path. These may be not the value that you hope and not good for re-calculation. But, it's useful for dump and monitor.