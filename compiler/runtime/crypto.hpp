#include <openssl/md5.h>

eve_string md5(const eve_string& buffer)
{
	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5(string.c_str(), string.length(), digest); 

	eve_string result;
	result.reserve(32);

	for (int i = 0; i < 16; i++)
		sprintf(&result.c_str()[i*2], "%02x", (unsigned int) digest[i]);

	return result;
}