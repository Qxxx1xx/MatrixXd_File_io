#include <iostream>
#include <Eigen/Dense>
#include <fstream>



/**
 * @brief 将矩阵 m 存储为 path 处的二进制文件。
 * 矩阵的类型包括：固定大小类型，动态大小类型，数据类型可以为（double、int、float）的矩阵。
 *
 * @param Scalar 矩阵数据类型
 * @param Rows 行数
 * @param Cols 列数
 * @param Major 数据存储方式
 * @param path 想要保存为二进制文件的路径
 * @param m 想要存储的矩阵，不能用 const 限定
 *
 * @return 是否保存成功
 * @retval bool
 */
template <class Scalar, int Rows, int Cols, int Major>
bool saveMatrix2BinaryFile(std::string path, Eigen::Matrix<Scalar, Rows, Cols, Major>& m)
{
    std::ofstream file(path.c_str(), std::ios::binary);

    // 使用 4 个 tag 来分别标记矩阵的数据类型、行数、列数、行主还是列主
    uint64_t scalar_type_tag;
    if (std::is_same_v<Scalar, double>)
        scalar_type_tag = 0;
    if (std::is_same_v<Scalar, int>)
        scalar_type_tag = 1;
    if (std::is_same_v<Scalar, float>)
        scalar_type_tag = 2;

    uint64_t rows = m.rows();
    uint64_t cols = m.cols();

    uint64_t major_tag{static_cast<uint64_t>(Major)};

    file.write(reinterpret_cast<char*>(&scalar_type_tag), sizeof(uint64_t));
    file.write(reinterpret_cast<char*>(&rows), sizeof(uint64_t));
    file.write(reinterpret_cast<char*>(&cols), sizeof(uint64_t));
    file.write(reinterpret_cast<char*>(&major_tag), sizeof(uint64_t));
    file.write(reinterpret_cast<char*>(m.data()), sizeof(Scalar) * m.size());

    file.close();
    return true;    
}

/** 
 * @brief 从二进制文件中加载动态大小的矩阵，数据类型为 Scalar，需要 scalar_type_tag 所示的数据类型与 Scalar 对应
 * 
 * @param Scalar 矩阵数据类型
 * @param path 二进制文件所在路径
 * @param m 数据加载到的矩阵
 * 
 * @return 是否从二进制文件中加载成功
 * @retval bool
*/
template <class Scalar>
bool loadMatrix_from_binaryFile(std::string path, Eigen::Matrix<Scalar, -1, -1>& m)
{
    if (std::ifstream file{path.c_str(), std::ios::binary | std::ios::ate}) {
        uint64_t filesize = static_cast<uint64_t>(file.tellg());
        uint64_t sizeofData = filesize - static_cast<uint64_t>(sizeof(uint64_t)) * 4; // 4个tag

        file.seekg(0);
        uint64_t scalar_type_tag;
        uint64_t rows;
        uint64_t cols;
        uint64_t major_tag;
        file.read(reinterpret_cast<char*>(&scalar_type_tag), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&rows), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&cols), sizeof(uint64_t));
        file.read(reinterpret_cast<char*>(&major_tag), sizeof(uint64_t));
        
        // 判断 scalar_type_tag 所示的数据类型是否与 Scalar 对应
        if (!((scalar_type_tag == 0 && std::is_same_v<Scalar, double>) ||
            (scalar_type_tag == 1 && std::is_same_v<Scalar, int>) ||
            (scalar_type_tag == 2 && std::is_same_v<Scalar, float>))
        ) {
            std::cout << "Scalar type mismath!" << std::endl;
            return false;
        }

        uint64_t numofScalar = sizeofData / sizeof(Scalar);
        Scalar* data_ptr = new Scalar[numofScalar];
        file.read(reinterpret_cast<char*>(&data_ptr[0]), sizeofData);
        if (major_tag == 0) {
            m = Eigen::Map<Eigen::Matrix<Scalar, -1, -1, Eigen::ColMajor>>(data_ptr, rows, cols);
        }
        else if (major_tag == 1) {
            m = Eigen::Map<Eigen::Matrix<Scalar, -1, -1, Eigen::RowMajor>>(data_ptr, rows, cols);
        }
        else {
            std::cout << "major_tag out of range!" << std::endl;
            return false;
        }
    }
    else {
        return false;
    }
    return true;
}

int main() {
    Eigen::Matrix<double, 3, 3, Eigen::ColMajor> m;
    m.setRandom();
    std::cout << m << std::endl << std::endl;
    saveMatrix2BinaryFile("testSave.dat", m);
    Eigen::MatrixXd mm;
    loadMatrix_from_binaryFile("testSave.dat", mm);
    std::cout << mm << std::endl;

    Eigen::Matrix<int, 3, 3, Eigen::RowMajor> mi;
    mi.setRandom();
    std::cout << mi << std::endl << std::endl;
    saveMatrix2BinaryFile("testSaveMi.dat", mi);
    Eigen::MatrixXi mmi;
    loadMatrix_from_binaryFile("testSaveMi.dat", mmi);
    std::cout << mmi << std::endl;

    Eigen::Matrix<float, 3, 3, Eigen::ColMajor> mf;
    mf.setRandom();
    std::cout << mf << "\n\n";
    saveMatrix2BinaryFile("testSaveMf.dat", mf);
    Eigen::MatrixXf mmf;
    loadMatrix_from_binaryFile("testSaveMf.dat", mmf);
    std::cout << mmf << std::endl;
    
}