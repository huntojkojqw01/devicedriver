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
#define DECODER_MAJOR 60
//RSA CONFIG:
#define N 13481
#define E 2143
#define D 4207
#define U 13


//RSA header:
typedef struct{
  uint data;  //  chứa các bit.
  int head;           //  đánh dấu đầu của bufer.
}buffer;

typedef struct{
  uint p,q,n,m,e,d,u;// các thuộc tính trong thuật toán RSA.
}rsa_params;

buffer        sbuff,dbuff;  //  sbuff là buffer của xâu nguồn,dbuff là buffer của xâu đích.
int           num_of_bit;   //  số lượng bit của mỗi khối sẽ đem đi mã hóa bằng RSA.
char          *sp,*dp;      //  các con trỏ đến xâu nguồn, xâu đích.
uint          plain,code;   //  plain là dữ liệu gốc, code là dữ liệu đã được mã hóa.
rsa_params    _rsa;         //  biến chứa các thuộc tính của thuật toán RSA.

void  init(void);                     // hàm khởi tạo các thuộc tính cho thuật toán RSA.

uint  power(uint x,uint y,uint mod);  // tính x^y modul n 

uint  rsa_decode(int y);              // giải mã một số y thành x=y^d modul n.

void  giai_ma(char* src, char* des);  // giải mã xâu nguồn, ghi vòa xâu đích.
//-------------------------------------------------------------------------------------


MODULE_LICENSE("Dual BSD/GPL");



/* Declaration of decoder.c functions */

int decoder_open(struct inode *inode, struct file *filp);

int decoder_release(struct inode *inode, struct file *filp);

ssize_t decoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t decoder_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

static int decoder_init(void);

static void decoder_exit(void);

/* Structure that declares the usual file */

/* access functions */

struct file_operations decoder_fops = {

  read: decoder_read,

  write: decoder_write,

  open: decoder_open,

  release: decoder_release

};



/* Declaration of the init and exit functions */

module_init(decoder_init);

module_exit(decoder_exit);



/* Global variables of the driver */

/* Major number */

int decoder_major = DECODER_MAJOR;

/* Buffer to store data */

char *decoder_buffer;
char *rsa_buffer;


static int  decoder_init(void) {
  int result;  

  /* Registering device */
  result = register_chrdev(decoder_major, "decoder", &decoder_fops);

  if (result < 0) {
    printk(
      "<1>decoder: cannot obtain major number %d\n", decoder_major);
    return result;

  }
  /* Allocating decoder for the buffer */

  decoder_buffer = kmalloc(BUFF_SIZE, GFP_KERNEL);
  rsa_buffer = kmalloc(BUFF_SIZE, GFP_KERNEL); 

  if (!decoder_buffer || !rsa_buffer) { 

    result = -ENOMEM;

    goto fail; 

  } 
  memset(decoder_buffer, 0, BUFF_SIZE);
  memset(rsa_buffer, 0, BUFF_SIZE);

  printk("<1>Inserting decoder module\n");

  init();  
  
  printk("RSA attributes: n=%d, e=%d, d=%d, u=%d\n",_rsa.n, _rsa.e, _rsa.d, _rsa.u);      
  
  return 0;



  fail: 
    decoder_exit(); 
    return result;
}

static void decoder_exit(void) {

  /* Freeing the major number */
  unregister_chrdev(decoder_major, "decoder");

  /* Freeing buffer decoder */
  if (decoder_buffer) {
    kfree(decoder_buffer);
  }
  if (rsa_buffer) {
    kfree(rsa_buffer);
  }
  printk("<1>Removing decoder module\n");
}

int decoder_open(struct inode *inode, struct file *filp){
	/* Success */
  return 0;
}

int decoder_release(struct inode *inode, struct file *filp){
	/* Success */
  return 0;
}

ssize_t decoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	/* Transfering data to user space */   
  if(*f_pos < BUFF_SIZE - 1){
    copy_to_user(buf, decoder_buffer + *f_pos, 1);
    (*f_pos) += 1;
  }  
  if(count == 1){
    *(buf + *f_pos) = 0;
    *f_pos = 0;    
    return 0;
  }
  else{
    return 1;
  }
}

ssize_t decoder_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){	  
  if(*f_pos < BUFF_SIZE-1){
    copy_from_user(rsa_buffer + *f_pos, buf, 1);
    (*f_pos) += 1;          
  }
  if(count == 1){
    *(rsa_buffer + *f_pos) = 0;
    *f_pos = 0;    
    giai_ma(rsa_buffer, decoder_buffer);    
  }    
  return 1;
}

//RSA FUNCTION:
uint power(uint x, uint y, uint mod){// tính x mũ y modul mod
  uint a, tmp;
  if(y == 0) return 1 % mod;
  else
    if(y == 1) return x % mod;
    else {
      a = y / 2;
      tmp = (x * x) % mod;
      if(y % 2 == 0)
        return power(tmp, a, mod);
      else{
        tmp = power(tmp, a, mod);
        return (tmp * x) % mod;
      }
    } 
}

void init(void){
  _rsa.n = N, _rsa.e = E, _rsa.d = D, _rsa.u = U;
}

uint rsa_decode(int y){
  return power(y, _rsa.d, _rsa.n);
}

void src_buff_enqueue(void){// hàm này thực hiện đọc dữ liệu từ string rồi chèn nó vào sau src buffer-data.
  unsigned char c;
  while(sbuff.head < num_of_bit && sbuff.head + BYTE_LEN < BUFFER_LEN && *sp ){    
    if((c = *sp++) > 0){
      sbuff.head += sizeof(char) << 3;
      sbuff.data = (sbuff.data << BYTE_LEN) | c;      
    }
    else{     
      break;        
    }
  } 
}

uint src_buff_dequeue(void){// hàm này trả về (num_of_bit) đầu tiên của xâu nguồn.
  uint result, tmp = (2 << num_of_bit) - 1;// tạo ra một số tmp gồm (num_of_bit) các bit 1.
  if(sbuff.head >= num_of_bit){
    result = sbuff.data >> (sbuff.head = sbuff.head - num_of_bit);// result có giá trị bằng (num_of_bit) đầu tiên của data. 
    sbuff.data &= ~(tmp << sbuff.head);// xóa (num_of_bit) đầu tiên của data về giá trị 0.     
    return result;
  }else return MAX_VAL;
}

void des_buff_enqueue(uint value){// hàm này thực hiện ghi một (num_of_bit) bit dữ liệu có giá trị value vào des buffer-data.
  if(dbuff.head + num_of_bit < BUFFER_LEN){
    dbuff.head += num_of_bit;
    dbuff.data = (dbuff.data << num_of_bit) | value;    
  } 
}

void des_buff_dequeue(void){// hàm này lấy ra (num_of_bit) dữ liệu từ des buffer ghi vào xâu đích.
  unsigned char c;  
  while(dbuff.head >= BYTE_LEN){    
    c = dbuff.data>> (dbuff.head = dbuff.head - BYTE_LEN);// c có giá trị bằng (num_of_bit) đầu tiên của data.    
    *dp++ = c;   
    dbuff.data &= ~(FF << dbuff.head);// xóa 8 bit đầu tiên của data về giá trị 0.       
  } 
}

void quet_sach(void){//chuyển nốt các bit còn sót trong src buffer vào des buffer
  if(sbuff.head > 0){
    dbuff.head += sbuff.head;
    dbuff.data = (dbuff.data << sbuff.head) | sbuff.data;
    sbuff.data >>= sbuff.head, sbuff.head = 0;   
  }
  des_buff_dequeue();  
}

void giai_ma(char* src, char* des){
  memset(&sbuff, 0, sizeof(buffer));
  memset(&dbuff, 0, sizeof(buffer));  
  num_of_bit = _rsa.u, sp = src, dp = des;  
  while(*sp){
    src_buff_enqueue();
    code = src_buff_dequeue();
    if(code < MAX_VAL){     
      plain = rsa_decode(code);//thực hiện giải mã.
      while(plain > (2 << (num_of_bit - 1))) plain = rsa_decode(plain);//chống tràn bit sau lũy thừa.     
      des_buff_enqueue(plain);    
    }   
    des_buff_dequeue();      
  }
  quet_sach();
  *sp = *dp = '\0';
}