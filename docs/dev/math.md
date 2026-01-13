# 数学函数约定

基本上与`Eigen`库的函数声明兼容

```cpp

class MathObject
{
public:
    static MathObject Identity();

    MathObject normalized() const;
    void normalize();

    MathObject inverse() const;
    void getInverse(MathObject& me) const;

    Self transpose() const;
    void transposeInPlace();

};

```

`MathObject`是下面的其中一种：`Matrix`、`Vector`、`Quaternion`、`Rotation`、`Transform`等等