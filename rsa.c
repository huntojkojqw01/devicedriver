#include<linux/string.h>
#include "rsa.h"
#include "consant.h"

unsigned int luy_thua_cao(unsigned int x,unsigned int y,unsigned int mod){// tính x mũ y modul mod
	unsigned int a,b,tmp;
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
	result=(int*)malloc(3*sizeof(int));// result là mảng trả về chứa g,a,b
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

}
void init(rsa_params *x){
	srand(time(NULL));
	//vòng lặp để chọn ra p và q:
	do{
		x->p=primes[rand()%50],	x->q=primes[rand()%50];// nếu chọn p, q to quá thì n sẽ to và khi tính Y^d bị lòi
	}while(x->p==x->q||x->p*x->q<256);	
	x->n=x->p*x->q,x->m=(x->p-1)*(x->q-1);

	//vòng lặp để chọn ra e:
	do{
		x->e=primes[rand()%1000];
	}while(x->e>x->m-1||x->m%x->e==0);

	//dùng thuật toán gcd mở rộng để tìm ra d:
	int *tmp;
	tmp=gcd_mo_rong(x->e,x->m);
	if(tmp[1]<0) x->d=tmp[1]+x->m;// nếu d bị âm ta phải + với m để thành dương.
	else x->d=tmp[1];
	//tính số bit mỗi lần mã hóa, phải thóa 2^u < n-1.
	int u=0;
	while(2<<u++ < x->n-1);	x->u=u-1;
}
unsigned int rsa_encode(int x,rsa_params _rsa){
	return luy_thua_cao(x,_rsa.e,_rsa.n);
}
unsigned int rsa_decode(int y,rsa_params _rsa){
	return luy_thua_cao(y,_rsa.d,_rsa.n);
}

extern buffer sbuff,dbuff;//sbuff là buffer của file nguồn,dbuff là buffer của file đích.
extern int num_of_bit;// số lượng bit của mỗi khối sẽ đem đi mã hóa bằng RSA.
extern FILE *sf,*df;
extern char *sf2,*df2;
extern unsigned int plain,code;

void src_buff_enqueue2(){// hàm này thực hiện đọc dữ liệu từ string rồi chèn nó vào sau src buffer-data.
	unsigned char c;
	while(sbuff.head<num_of_bit && sbuff.head+BYTE_LEN < BUFFER_LEN && *sf2 ){		
		if((c=*sf2++)>0){
			sbuff.head+=sizeof(char)<<3;
			sbuff.data=(sbuff.data<<BYTE_LEN)|c;			
		}
		else{
			puts("Xau het du lieu.");
			break;				
		}
	}	
}

unsigned int src_buff_dequeue(){// hàm này trả về (num_of_bit) đầu tiên của file f.
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
	else{			
		puts("Tran des buff!");		
	}	
}

void des_buff_dequeue2(){	
	unsigned char c;	
	while(dbuff.head>=BYTE_LEN){		
		c= dbuff.data>> (dbuff.head= dbuff.head-BYTE_LEN);// c có giá trị bằng (num_of_bit) đầu tiên của data.		
		*df2++=c;		
		dbuff.data &=~(FF<<dbuff.head);// xóa 8 bit đầu tiên của data về giá trị 0.				
	}	
}

void quet_sach2(){//chuyển nốt các bit còn sót trong src buffer vào des buffer
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