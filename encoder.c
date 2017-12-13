/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> /* file_operations */
#include <linux/slab.h> /* kmalloc() */
#include <asm/uaccess.h> /* copy_from/to_user */

#define BYTE_LEN 8
#define BUFFER_LEN 32
#define MAX_VAL 2147483647
#define SUCCESS 1
#define FAILED 0
#define FF 511// 8 bit 1 lien tiep
#define BUFF_SIZE 1000
#define P 221
#define Q 61
#define E 2143

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
//-------------------------------------------------------------------------------------

MODULE_LICENSE("Dual BSD/GPL");



/* Declaration of encoder.c functions */

int encoder_open(struct inode *inode, struct file *filp);

int encoder_release(struct inode *inode, struct file *filp);

ssize_t encoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t encoder_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

static int encoder_init(void);

static void encoder_exit(void);

/* Structure that declares the usual file */

/* access functions */

struct file_operations encoder_fops = {

  read: encoder_read,

  write: encoder_write,

  open: encoder_open,

  release: encoder_release

};



/* Declaration of the init and exit functions */

module_init(encoder_init);

module_exit(encoder_exit);



/* Global variables of the driver */

/* Major number */

int encoder_major = 50;

/* Buffer to store data */

char *encoder_buffer;
char *rsa_buffer;
rsa_params _rsa;


static int  encoder_init(void) {
  int result,x,y;
  char src[10],des[10];

  /* Registering device */
  result = register_chrdev(encoder_major, "encoder", &encoder_fops);

  if (result < 0) {
    printk(
      "<1>encoder: cannot obtain major number %d\n", encoder_major);
    return result;

  }
  /* Allocating encoder for the buffer */

  encoder_buffer = kmalloc(BUFF_SIZE, GFP_KERNEL);
  rsa_buffer = kmalloc(BUFF_SIZE, GFP_KERNEL); 

  if (!encoder_buffer || !rsa_buffer) { 

    result = -ENOMEM;

    goto fail; 

  } 
  memset(encoder_buffer, 0, BUFF_SIZE);
  memset(rsa_buffer, 0, BUFF_SIZE);

  printk("<1>Inserting encoder module\n");

  init(&_rsa);  
  x=123;
  printk("RSA attributes: p=%d, q=%d, n=%d, m=%d, e=%d, d=%d, u=%d\n",_rsa.p,_rsa.q,_rsa.n,_rsa.m,_rsa.e,_rsa.d,_rsa.u);      
  y=rsa_encode(x,_rsa);
  printk("Test number: %d --> %d --> %d\n",x,y,rsa_decode(y,_rsa)); 
      
  strcpy(src,"abcdefhg");
  ma_hoa2(_rsa,src,des);  
  printk("Test string: \"%s\" encode--> \"%s\" \n",src,des);
  strcpy(src,des);
  giai_ma2(_rsa,src,des);
  printk("Test string: \"%s\" decode--> \"%s\" \n",src,des);
  return 0;



  fail: 
    encoder_exit(); 
    return result;
}

static void encoder_exit(void) {

  /* Freeing the major number */
  unregister_chrdev(encoder_major, "encoder");

  /* Freeing buffer encoder */
  if (encoder_buffer) {
    kfree(encoder_buffer);
  }
  if (rsa_buffer) {
    kfree(rsa_buffer);
  }
  printk("<1>Removing encoder module\n");
}

int encoder_open(struct inode *inode, struct file *filp){
	/* Success */
  return 0;
}

int encoder_release(struct inode *inode, struct file *filp){
	/* Success */
  return 0;
}

ssize_t encoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	/* Transfering data to user space */   
  if(*f_pos<BUFF_SIZE-1){
    copy_to_user(buf,encoder_buffer+*f_pos,1);
    (*f_pos)+=1;
  }  
  if(count==1){
    *(buf+*f_pos)=0;
    *f_pos=0;    
    return 0;
  }
  else{
    return 1;
  }
}

ssize_t encoder_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){	  
  if(*f_pos<BUFF_SIZE-1){
    copy_from_user(rsa_buffer+*f_pos,buf,1);
    (*f_pos)+=1;          
  }
  if(count==1){
    *(encoder_buffer+*f_pos)=0;
    *f_pos=0;
    ma_hoa2(_rsa,rsa_buffer,encoder_buffer);    
  }    
  return 1;
}

//=================================RSA LIB==========
unsigned int luy_thua_cao(unsigned int x,unsigned int y,unsigned int mod){// tính x mũ y modul mod
  unsigned int a,tmp;
  if(y==0) return 1%mod;
  else
    if(y==1) return x%mod;
    else {
      a=y/2;
      tmp=(x*x)%mod;
      if(y%2==0)
        return luy_thua_cao(tmp,a,mod);
      else{
        tmp=luy_thua_cao(tmp,a,mod);
        return (tmp*x)%mod;
      }
    } 
}
int* gcd_mo_rong(int n1,int n2){
  register int a1=1,b2=1,a2=0,b1=0,q,r,t;
  int *result;
  result=(int*)kmalloc(3*sizeof(int), GFP_KERNEL);// result là mảng trả về chứa g,a,b
  do{
    q=n1/n2,r=n1%n2;
    if(r==0){
      result[0]=n2,result[1]=a2,result[2]=b2;     
      return result;    
    }
    else{
      n1=n2,n2=r,t=a2,a2=a1-q*a2,a1=t,t=b2,b2=b1-q*b2,b1=t;
    }
  }while(r!=0);
  return result;
}
void init(rsa_params *x){
  int *tmp, u=0;

  x->p=P, x->q=Q;   
  x->n=x->p*x->q, x->m=(x->p-1)*(x->q-1);
  x->e=E; 

  //dùng thuật toán gcd mở rộng để tìm ra d:  
  tmp=gcd_mo_rong(x->e,x->m);
  if(tmp[1]<0) x->d=tmp[1]+x->m;// nếu d bị âm ta phải + với m để thành dương.
  else x->d=tmp[1];

  //tính số bit mỗi lần mã hóa, phải thóa 2^u < n-1.  
  while(2<<u++ < x->n-1); x->u=u-1;
}
unsigned int rsa_encode(int x,rsa_params _rsa){
  return luy_thua_cao(x,_rsa.e,_rsa.n);
}
unsigned int rsa_decode(int y,rsa_params _rsa){
  return luy_thua_cao(y,_rsa.d,_rsa.n);
}

void src_buff_enqueue2(void){// hàm này thực hiện đọc dữ liệu từ string rồi chèn nó vào sau src buffer-data.
  unsigned char c;
  while(sbuff.head<num_of_bit && sbuff.head+BYTE_LEN < BUFFER_LEN && *sf2 ){    
    if((c=*sf2++)>0){
      sbuff.head+=sizeof(char)<<3;
      sbuff.data=(sbuff.data<<BYTE_LEN)|c;      
    }
    else{     
      break;        
    }
  } 
}

unsigned int src_buff_dequeue(void){// hàm này trả về (num_of_bit) đầu tiên của file f.
  unsigned int result, tmp=(2<<num_of_bit)-1;// tạo ra một số tmp gồm (num_of_bit) các bit 1
  if(sbuff.head>=num_of_bit){
    result= sbuff.data>>(sbuff.head= sbuff.head-num_of_bit);// result có giá trị bằng (num_of_bit) đầu tiên của data. 
    sbuff.data &=~(tmp<<sbuff.head);// xóa (num_of_bit) đầu tiên của data về giá trị 0.     
    return result;
  }else return MAX_VAL;
}

void des_buff_enqueue(unsigned int value){// hàm này thực hiện ghi một (num_of_bit) bit dữ liệu có giá trị value vào des buffer-data.
  if(dbuff.head+num_of_bit<BUFFER_LEN){
    dbuff.head+=num_of_bit;
    dbuff.data=(dbuff.data<<num_of_bit)|value;    
  } 
}

void des_buff_dequeue2(void){ 
  unsigned char c;  
  while(dbuff.head>=BYTE_LEN){    
    c= dbuff.data>> (dbuff.head= dbuff.head-BYTE_LEN);// c có giá trị bằng (num_of_bit) đầu tiên của data.    
    *df2++=c;   
    dbuff.data &=~(FF<<dbuff.head);// xóa 8 bit đầu tiên của data về giá trị 0.       
  } 
}

void quet_sach2(void){//chuyển nốt các bit còn sót trong src buffer vào des buffer
  if(sbuff.head>0){
    dbuff.head+=sbuff.head;
    dbuff.data=(dbuff.data<<sbuff.head)|sbuff.data;
    sbuff.data>>=sbuff.head,sbuff.head=0;   
  }
  des_buff_dequeue2();  
}

void ma_hoa2(rsa_params _rsa,char* src,char* des){
  memset(&sbuff,0,sizeof(buffer));
  memset(&dbuff,0,sizeof(buffer));  
  num_of_bit=_rsa.u;  
  sf2=src;  
  df2=des;  
  while(*sf2!='\0'){
    src_buff_enqueue2();
    plain=src_buff_dequeue();
    if(plain<MAX_VAL){          
      code=rsa_encode(plain,_rsa);//thuc hien viec ma hoa
      while(code>(2<<(num_of_bit-1))) code=rsa_encode(code,_rsa);//chong tran bit sau luy thua      
      des_buff_enqueue(code);   
    }   
    des_buff_dequeue2();      
  }
  quet_sach2();
  *sf2=*df2='\0'; 
}

void giai_ma2(rsa_params _rsa,char* src,char* des){
  memset(&sbuff,0,sizeof(buffer));
  memset(&dbuff,0,sizeof(buffer));  
  num_of_bit=_rsa.u;  
  sf2=src;
  df2=des;  
  while(*sf2){
    src_buff_enqueue2();
    code=src_buff_dequeue();
    if(code<MAX_VAL){     
      plain=rsa_decode(code,_rsa);//thuc hien viec giai ma
      while(plain>(2<<(num_of_bit-1))) plain=rsa_decode(plain,_rsa);//chong tran bit sau luy thua     
      des_buff_enqueue(plain);    
    }   
    des_buff_dequeue2();      
  }
  quet_sach2();
  *sf2=*df2='\0';
}