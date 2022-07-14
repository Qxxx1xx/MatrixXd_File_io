# Eigen::Matrix 与二进制文件的输入输出

#Eigen

## 需求分析

- 所有矩阵和向量都是`Matrix`模板类的对象，`Matrix`类有6个模板参数

```c++
Matrix<typename Scalar, 
       int RowsAtCompileTime, 
       int ColsAtCompileTime,
       int Options = 0,
       int MaxRowsAtCompileTime = RowsAtCompileTime,
       int MaxColsAtCompileTime = ColsAtCompileTime>
# Scalar 元素类型
# RowsAtCompileTime 行
# ColsAtCompileTime 列
# 例 typedef Matrix<int, 3, 3> Matrix3i;
# Options 比特标志位——行主还是列主
# MaxRowsAtCompileTime和MaxColsAtCompileTime表示在编译阶段矩阵的上限。
```

- 前三个是必须指定的，后三个可以使用默认值。
- 但是有个情况是，输出的时候为行主存储的矩阵，输入的时候存储为列主的矩阵，就会得到我们不想得到的结果。应该要区分这个情况。
- 因此在文件开头提供四个信息，获得存储的矩阵的具体类型，来正确加载。
- 二进制文件不像文本文件所见即所得，可以理解为将所有信息都放在一行上面，紧密的排列着。
- 而且不同类型指针在同一个位置读取，会得到不同结果。
- 因此保存的信息需要是我们已知大小的类型，如 uint64_t 类型是 8 字节大小，double 类型是 8 字节大小。而 int 类型不是一个标准类型，在不同编译器下体现的字节数不同。
- 这里使用 uint64_t 类型来保存矩阵的四个信息。

## Eigen::Matrix 输出为二进制文件

- 实数数据类型的矩阵闭着眼睛调用

- 适用于：

  - 数据类型为：double, int, float
  - 矩阵大小为：固定大小，动态大小
  - 矩阵数据存储方式：行主、列主

- 主要用到 std::basic_ostream std::basic_ostream::write( const char_type* s, [std::streamsize](http://en.cppreference.com/w/cpp/io/streamsize) count ); 这个函数。

  - 两个参数：首地址和字节数

  - ```c++
    #include <iostream>
     
    int main()
    {
        char c[]="This is sample text.";
        std::cout.write(c, 4).write("!\n", 2);
    }
    ```

  - ```powershell
    This!
    ```

- 可以这样用

  - ```c++
    #include <ofstream>
    
    int main() {
        std::vector<double> v{1.0, 2.0, 3.0};
        std::ofstream file("test.dat", std::ios::binary);
        file.write(reinterpret_cast<char*>(v.data()), v.size() * sizeof(double));
    }
    ```

  - std::vector 中的数据都是紧密排列，通过 v.data() 获得数据首地址，字节数为 v.size() * sizeof(double)，将输出 std::vector 中的所有数据

## Eigen::Matrix 从二进制文件中加载

- 不像输出一样，一个矩阵闭着眼睛调用该函数就可以
- 该函数调用者需要心里有数，被加载的二进制文件中的数据类型是什么，并且相同数据类型的矩阵作为函数输入

