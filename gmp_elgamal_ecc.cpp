#include <bits/stdc++.h>
#include <gmp.h>
using namespace std;

#define P "1253565240884932978867327564152117754558173168651037992019849"
#define A "602060795339016484143949354474116343558714560760195967177227"
#define B "42352398925875963961914576668959717307495502739895755462837"
#define XG "308713420171208600284839010152791755784186567349039150579062"
#define YG "494952139410905522093322548267656122025755798264866184081137"
#define N "9217391477095095432847996795221681564891374309161846704797"

void decrypt(mpz_t Xc1, mpz_t Yc1, mpz_t C2_points[][2], mpz_t k, mpz_t p,long long int l, string *dec_msg)
{
	mpz_t Xm,Ym,decrypt_points[l][2];
	mpz_inits(Xm,Ym, NULL);
	
	// Calculating (Xm,Ym) = ((Xc1*k)%p , (Yc1*k)%p)
	mpz_mul(Xm, k, Xc1);
	mpz_mod(Xm, Xm, p);
	mpz_mul(Ym, k, Yc1);
	mpz_mod(Ym, Ym, p);

    // Calculating decrypted points
	for (long long int i=0;i<l;i++)
	{
		mpz_inits(decrypt_points[i][0], decrypt_points[i][1], NULL);
		mpz_sub(decrypt_points[i][0], C2_points[i][0], Xm);
		mpz_mod(decrypt_points[i][0], decrypt_points[i][0], p);
		mpz_sub(decrypt_points[i][1], C2_points[i][1], Ym);
		mpz_mod(decrypt_points[i][1], decrypt_points[i][1], p);
	}

    // Decoding points to message
	for (int i=0;i<l;i++)
	{
		char ch[10];
		mpz_get_str(ch,10,decrypt_points[i][0]);
		int t= atoi(ch);
		char ch1=t;
		*dec_msg+=ch1;
	}
}
void encrypt(char message[], mpz_t a, mpz_t b, mpz_t p, mpz_t Xg, mpz_t Yg,
             mpz_t Xq, mpz_t Yq, mpz_t *Xc1, mpz_t *Yc1, mpz_t C2_points[][2])
{
    int temp;
    mpz_t mpz_temp,mpz_temp1,mpz_temp2,msg_points[strlen(message)][2];
    mpz_inits(mpz_temp,mpz_temp1,mpz_temp2, NULL);
    
    // Encoding message into points on elliptic curve
    for (int i=0;i<strlen(message);i++)
    {
    	temp = message[i];
    	char ch[10];
    	itoa(temp,ch,10);
    	// setting x = ascii value of character
    	mpz_init_set_str(msg_points[i][0],ch , 0);
    	
    	// Calculating y value taking ascii value as x
    	mpz_mul(mpz_temp, msg_points[i][0], msg_points[i][0]);
    	mpz_mul(mpz_temp, mpz_temp, msg_points[i][0]);
    	mpz_mul(mpz_temp1, msg_points[i][0], a);
    	mpz_add(mpz_temp2, mpz_temp1,b);
    	mpz_inits(msg_points[i][1], NULL);
    	mpz_add(msg_points[i][1],mpz_temp2,mpz_temp);
    	mpz_sqrt(msg_points[i][1],msg_points[i][1]);
	}

    // generating r randomly
	gmp_randstate_t r_state;
	mpz_t r;
	mpz_inits(r, NULL);
	gmp_randinit_mt(r_state);
	srand(time(0));
	int seed = rand();
	gmp_randseed_ui(r_state, seed);
	mpz_urandomb(r, r_state, 100);
	
	// Calculating C1(x,y) = r*G(x,y)
	mpz_mul(*Xc1, r, Xg);
	mpz_mod(*Xc1, *Xc1, p);
	mpz_mul(*Yc1, r, Yg);
	mpz_mod(*Yc1, *Yc1, p);
	
	// Calculating C2(x,y) = r*Q(x,y) + Pm(x,y)
	for (int i=0;i<strlen(message);i++)
	{
		mpz_inits(C2_points[i][0], C2_points[i][1], NULL);
		mpz_mul(C2_points[i][0], r, Xq);
		mpz_add(C2_points[i][0], C2_points[i][0], msg_points[i][0]);
		mpz_mul(C2_points[i][1], r, Yq);
		mpz_add(C2_points[i][1], C2_points[i][1], msg_points[i][1]);
	}
}
int main()
{
	char message[10000];
	mpz_t p,a,b,Xg,Yg,n,k,Xq,Yq;
	mpz_t msg_points[10000][2];
	gmp_randstate_t k_state;
	mpz_inits(p,a,b,Xg,Yg,n, NULL);
	
	// Setting p,a,b,G,n values
	mpz_set_str(p,P,10);
	mpz_set_str(a,A,10);
	mpz_set_str(b,B,10);
	mpz_set_str(Xg,XG,10);
	mpz_set_str(Yg,YG,10);
	mpz_set_str(n,N,10);

	cout<<"\n-------------------------------------------------------------------------------------------------------------\n";
	gmp_printf("\n Elliptic Curve E(a,b): y2 = x3 + (%Zd)x + %Zd", a, b);
	gmp_printf("\n Generator G(x,y): (%Zd,%Zd)", Xg,Yg);
	gmp_printf("\n p:%Zd n:%Zd", p, n);
	
	//generating private key randomly
	mpz_inits(k,Xq,Yq, NULL);
	gmp_randinit_mt(k_state);
	srand(time(0));
	int seed = rand();
	gmp_randseed_ui(k_state, seed);
	mpz_urandomb(k, k_state, 1000);

	gmp_printf("\n\n Private key: %Zd", k);
	mpz_mul(Xq,k,Xg);
	mpz_mul(Yq,k,Yg);
	gmp_printf("\n\n Public key Q(x,y): (%Zd  ,  %Zd)", Xq,Yq);

    cout<<"\n-------------------------------------------------------------------------------------------------------------\n";
    cout<<"Enter msg to send:";cin.getline(message,10000);
	
	mpz_t Xc1,Yc1,C2_points[strlen(message)][2];
	mpz_inits(Xc1,Yc1, NULL);
	
	// Calling encryption
	encrypt(message,a,b,p,Xg,Yg,Xq,Yq,&Xc1,&Yc1,C2_points);
	cout<<"\n-------------------------------------------------------------------------------------------------------------\n";
	gmp_printf("\nC1(x,y): (%Zd, %Zd)", Xc1, Yc1);
	cout<<endl<<"C2(x,y):";
	for (int i=0;i<strlen(message);i++)
	{
		gmp_printf("\n (%Zd  ,  %Zd)", C2_points[i][0],C2_points[i][1]);
	}
	
	string dec_msg="";
	// calling decryption
	decrypt(Xc1,Yc1,C2_points,k,p,strlen(message), &dec_msg);
	cout<<"\n-------------------------------------------------------------------------------------------------------------\n";
    cout<<"\n Decrypted message:"<<dec_msg;
}
