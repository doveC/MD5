#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ԥ������������ԭ��buffer���������յ�Ҫɢ�е���Ϣ��
// ��������λ�Լ����ϳ��Ⱥ����Ϣ������Ϊ512λ�ı���
// ����Ͳ���length������Ϣ��������Ĵ�С
unsigned int* preprocess(char* buff, size_t* length) {
	// ��������λ����������Ҫɢ�е���Ϣ��λ��
	// ��������λ�� + ��λ + 64 = 512�ı���
	//       ����Ҫ�в�λ��������Ӳ�λ�պ���512�ı����ͼ�512λ�Ĳ�λ
	// ����Ҫ����λ��Ϊ��512 - (����λ�� + 64) % 512
	// ��������Ҫɢ�е���Ϣ��λ��Ϊ ����λ�� + ��λ + 64
	size_t input_bits = strlen(buff) * 8;
	size_t fills = 512 - (input_bits + 64) % 512;
	size_t total_bits = input_bits + fills + 64;

	unsigned int* message = (unsigned int*)malloc(total_bits / 8);  // ����Ҫɢ�е���Ϣ��total_bitsλ��ǿת��int�����������
	if (message == NULL) {
		perror("malloc error");
		exit(-1);
	}

	// �����ռ����Ϊ0
	memset(message, 0, total_bits / 8);	

	// 1. ��ԭ��ֱ�Ӹ��Ƶ����ռ�
	memcpy(message, buff, strlen(buff));

	// 2. ��䲹λ��������Ƭ�ռ��Ѿ���ʼ��Ϊ0�ˣ���������ֻ��Ҫ��ԭ�ĺ����һλ��һ��1
	//    ������ǣ���message���ڵĵ�ַ���� input_bits/8 �ֽڵ��Ǹ��ֽڵ����λ��Ϊ1
	*((unsigned char*)message + input_bits / 8) |= 1 << 7;

	// 3. ������Ϣԭ���message�����64λ�У�������input_bits
	// ��64λ�޷���ʾ����ԭ������ 2^64 λ����ȡ���ȵ����64λ
	message[total_bits / 32 - 2] += input_bits;  // ��64λ�����ݼӵ�32λ�ռ��ڣ��Զ��ضϸ�32λ����
	message[total_bits / 32 - 1] += input_bits >> 32;  // ��32λ���ݼӵ�32λ�ռ���

	*length = total_bits / 32;
	return message;
}

// �����õ����ĸ���ͬ�ĸ�������
unsigned int P1(unsigned int x, unsigned int y, unsigned int z) {
	return (x & y) | ((~x) & z);
}

unsigned int P2(unsigned int x, unsigned int y, unsigned int z) {
	return (x & z) | (y & (~z));
}

unsigned int P3(unsigned int x, unsigned int y, unsigned int z) {
	return x ^ y ^ z;
}

unsigned int P4(unsigned int x, unsigned int y, unsigned int z) {
	return y ^ (x | (~z));
}

// ���帨�������ĺ���ָ�����ͣ�����ص�
typedef unsigned int(*func)(unsigned int, unsigned int, unsigned int);

// ��unsigned intѭ������sλ
unsigned int circularly_left_shift(unsigned int x, int s) {
	s %= 32;

	x = x << s | x >> (32 - s);

	return x;
}

// ��������
void iteration(unsigned int* a, unsigned int b, unsigned int c, unsigned int d,
	unsigned int M, unsigned int s, unsigned int t, func P) {
	*a = circularly_left_shift((P(b, c, d) + *a + M + t), s) + b;
}

int main() {
	char buff[1024] = { 0 };
	printf("Please input the message: ");
	gets(buff);

	// ���Ƚ�ԭ�Ľ���Ԥ����
	size_t message_len = 0;
	unsigned int* message = preprocess(buff, &message_len);

	const unsigned int T[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
								 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8,
								 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
								 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51,
								 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
								 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905,
								 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681,
								 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60,
								 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
								 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
								 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
								 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314,
								 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
	};

	const unsigned int S[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
								 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
								 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
								 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	// ��ʼ�����ӱ���
	unsigned int A = 0x67452301;
	unsigned int B = 0xEFCDAB89;
	unsigned int C = 0x98BADCFE;
	unsigned int D = 0x10325476;

	// ��message��512λ�ֿ飬���ջ�ֵ� message_len * 32 / 512 ����
	// ����Ĺ���ѭ������ÿһ��512λ�飬����Ĳ�����ͬ
	for (int i = 0; i < message_len * 32 / 512; i++) {
		// ����512λ�Ŀ�
		// ��A,B,C,D���Ƶ�4������a,b,c,d��
		unsigned int a = A;
		unsigned int b = B;
		unsigned int c = C;
		unsigned int d = D;

		// ����ǰ��512λ��ֽ�Ϊ16���ӿ飬��֪ÿ���ӿ�λ32λ���պ���һ��unsigned int
		// ��16���ӿ�ֱ�Ϊ��message[i * 16 + 0]��message[i * 16 + 1]��...��message[i * 16 + 15]

		// ��ʱ�ٽ������ּ���
		iteration(&a, b, c, d, message[i * 16 + 0], S[0], T[0], P1);
		iteration(&d, a, b, c, message[i * 16 + 1], S[1], T[1], P1);
		iteration(&c, d, a, b, message[i * 16 + 2], S[2], T[2], P1);
		iteration(&b, c, d, a, message[i * 16 + 3], S[3], T[3], P1);
		iteration(&a, b, c, d, message[i * 16 + 4], S[4], T[4], P1);
		iteration(&d, a, b, c, message[i * 16 + 5], S[5], T[5], P1);
		iteration(&c, d, a, b, message[i * 16 + 6], S[6], T[6], P1);
		iteration(&b, c, d, a, message[i * 16 + 7], S[7], T[7], P1);
		iteration(&a, b, c, d, message[i * 16 + 8], S[8], T[8], P1);
		iteration(&d, a, b, c, message[i * 16 + 9], S[9], T[9], P1);
		iteration(&c, d, a, b, message[i * 16 + 10], S[10], T[10], P1);
		iteration(&b, c, d, a, message[i * 16 + 11], S[11], T[11], P1);
		iteration(&a, b, c, d, message[i * 16 + 12], S[12], T[12], P1);
		iteration(&d, a, b, c, message[i * 16 + 13], S[13], T[13], P1);
		iteration(&c, d, a, b, message[i * 16 + 14], S[14], T[14], P1);
		iteration(&b, c, d, a, message[i * 16 + 15], S[15], T[15], P1);

		iteration(&a, b, c, d, message[i * 16 + 1], S[16], T[16], P2);
		iteration(&d, a, b, c, message[i * 16 + 6], S[17], T[17], P2);
		iteration(&c, d, a, b, message[i * 16 + 11], S[18], T[18], P2);
		iteration(&b, c, d, a, message[i * 16 + 0], S[19], T[19], P2);
		iteration(&a, b, c, d, message[i * 16 + 5], S[20], T[20], P2);
		iteration(&d, a, b, c, message[i * 16 + 10], S[21], T[21], P2);
		iteration(&c, d, a, b, message[i * 16 + 15], S[22], T[22], P2);
		iteration(&b, c, d, a, message[i * 16 + 4], S[23], T[23], P2);
		iteration(&a, b, c, d, message[i * 16 + 9], S[24], T[24], P2);
		iteration(&d, a, b, c, message[i * 16 + 14], S[25], T[25], P2);
		iteration(&c, d, a, b, message[i * 16 + 3], S[26], T[26], P2);
		iteration(&b, c, d, a, message[i * 16 + 8], S[27], T[27], P2);
		iteration(&a, b, c, d, message[i * 16 + 13], S[28], T[28], P2);
		iteration(&d, a, b, c, message[i * 16 + 2], S[29], T[29], P2);
		iteration(&c, d, a, b, message[i * 16 + 7], S[30], T[30], P2);
		iteration(&b, c, d, a, message[i * 16 + 12], S[31], T[31], P2);

		iteration(&a, b, c, d, message[i * 16 + 5], S[32], T[32], P3);
		iteration(&d, a, b, c, message[i * 16 + 8], S[33], T[33], P3);
		iteration(&c, d, a, b, message[i * 16 + 11], S[34], T[34], P3);
		iteration(&b, c, d, a, message[i * 16 + 14], S[35], T[35], P3);
		iteration(&a, b, c, d, message[i * 16 + 1], S[36], T[36], P3);
		iteration(&d, a, b, c, message[i * 16 + 4], S[37], T[37], P3);
		iteration(&c, d, a, b, message[i * 16 + 7], S[38], T[38], P3);
		iteration(&b, c, d, a, message[i * 16 + 10], S[39], T[39], P3);
		iteration(&a, b, c, d, message[i * 16 + 13], S[40], T[40], P3);
		iteration(&d, a, b, c, message[i * 16 + 0], S[41], T[41], P3);
		iteration(&c, d, a, b, message[i * 16 + 3], S[42], T[42], P3);
		iteration(&b, c, d, a, message[i * 16 + 6], S[43], T[43], P3);
		iteration(&a, b, c, d, message[i * 16 + 9], S[44], T[44], P3);
		iteration(&d, a, b, c, message[i * 16 + 12], S[45], T[45], P3);
		iteration(&c, d, a, b, message[i * 16 + 15], S[46], T[46], P3);
		iteration(&b, c, d, a, message[i * 16 + 2], S[47], T[47], P3);

		iteration(&a, b, c, d, message[i * 16 + 0], S[48], T[48], P4);
		iteration(&d, a, b, c, message[i * 16 + 7], S[49], T[49], P4);
		iteration(&c, d, a, b, message[i * 16 + 14], S[50], T[50], P4);
		iteration(&b, c, d, a, message[i * 16 + 5], S[51], T[51], P4);
		iteration(&a, b, c, d, message[i * 16 + 12], S[52], T[52], P4);
		iteration(&d, a, b, c, message[i * 16 + 3], S[53], T[53], P4);
		iteration(&c, d, a, b, message[i * 16 + 10], S[54], T[54], P4);
		iteration(&b, c, d, a, message[i * 16 + 1], S[55], T[55], P4);
		iteration(&a, b, c, d, message[i * 16 + 8], S[56], T[56], P4);
		iteration(&d, a, b, c, message[i * 16 + 15], S[57], T[57], P4);
		iteration(&c, d, a, b, message[i * 16 + 6], S[58], T[58], P4);
		iteration(&b, c, d, a, message[i * 16 + 13], S[59], T[59], P4);
		iteration(&a, b, c, d, message[i * 16 + 4], S[60], T[60], P4);
		iteration(&d, a, b, c, message[i * 16 + 11], S[61], T[61], P4);
		iteration(&c, d, a, b, message[i * 16 + 2], S[62], T[62], P4);
		iteration(&b, c, d, a, message[i * 16 + 9], S[63], T[63], P4);

		// ����ʱ��a,b,c,d�ӻ�A,B,C,D
		A += a;
		B += b;
		C += c;
		D += d;
	}

	printf("md5: ");
	unsigned char* tmp = (unsigned char*)& A;
	printf("%2.2x%2.2x%2.2x%2.2x", tmp[0], tmp[1], tmp[2], tmp[3]);

	tmp = (unsigned char*)& B;
	printf("%2.2x%2.2x%2.2x%2.2x", tmp[0], tmp[1], tmp[2], tmp[3]);

	tmp = (unsigned char*)& C;
	printf("%2.2x%2.2x%2.2x%2.2x", tmp[0], tmp[1], tmp[2], tmp[3]);

	tmp = (unsigned char*)& D;
	printf("%2.2x%2.2x%2.2x%2.2x\n", tmp[0], tmp[1], tmp[2], tmp[3]);

	free(message);

	return 0;
}