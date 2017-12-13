/*
Tham khảo sách "Giáo trình cơ sở an toàn thông tin" thầy Nguyễn Khanh Văn,NXB Bách Khoa Hà Nội. 
*/
#ifndef _RSA_H
#define _RSA_H
#define BYTE_LEN 8
#define BUFFER_LEN 32
#define MAX_VAL 2147483647
#define SUCCESS 1
#define FAILED 0
#define FF 511// 8 bit 1 lien tiep
#define BUFF_SIZE 1000

/*
Việc mã hóa RSA thực hiện trên khối bit chứ không phải theo byte nên ta phải xây dựng 2 bộ đệm tương ứng cho 2 thao tác:
+ Đọc từng byte từ file nguồn lưu vào bộ đệm nguồn, khi nào đủ số bit của khối thì lấy ra để mã hóa rồi đưa vào bộ đệm đích,
+ Mỗi khi bộ đệm đích có đủ một byte dữ liệu trở lên thì sẽ ghi byte đó ra file đích.
*/
typedef struct{
	unsigned int data;// chứa các bit
	int head;// đánh dấu đầu của buffer
}buffer;
buffer sbuff,dbuff;//sbuff là buffer của file nguồn,dbuff là buffer của file đích.
int num_of_bit;// số lượng bit của mỗi khối sẽ đem đi mã hóa bằng RSA.
char *sf2,*df2;
unsigned int plain,code;

typedef struct{
	unsigned int p,q,n,m,e,d,u;// các tham số trong thuật toán RSA
}rsa_params;
/*
Thuật toán gcd mở rộng, áp dụng tìm nghịch đảo của n1 theo modulo n2,
kết quả trả về là một mảng [g,a,b] với g là UCLN của n1,n2 và a, b thỏa mãn
n1*a+n2*b=1.
Chi tiết tham khảo trang 71.
*/
int* gcd_mo_rong(int n1,int n2);
//------------------------------------------------
/*
Bước xây dựng các tham số p,q,n,e,m,d cho thuật toán RSA.
Chi tiết tham khảo trang 73.
*/
void init(rsa_params *x);
//----------
unsigned int luy_thua_cao(unsigned int x,unsigned int y,unsigned int mod);// tính x^y modul n 
//------------------------------------
unsigned int rsa_encode(int x,rsa_params _rsa);// mã hóa một số x thành y=x^e modul n.
//------------------------------------
unsigned int rsa_decode(int y,rsa_params _rsa);// giải mã một số y thành x=y^d modul n.
//------------------------------------
void ma_hoa2(rsa_params _rsa,char* src,char* des);// mã hóa một xau theo thuật RSA.
void giai_ma2(rsa_params _rsa,char* src,char* des);// giải mã một xau đã dc mã hóa RSA.
//--------------------------------------------
#endif //_RSA_H