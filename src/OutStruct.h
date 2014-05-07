/*
 the structure for output
 */
#ifndef _OUTSTRUCT_H_
#define _OUTSTRUCT_H_

struct OutStruct{
	__int64_t user1; //the first u_id;
	__int64_t user2; //the second u_id;
    std::set<std::string> tags;//to store tags;
	bool first;
	OutStruct()
	{
		first = true;
	}
    //insert tags into it
    
	void InsertTag(__int64_t user1, __int64_t user2, std::string tag)
	{
		if (first)
		{
			first = false;
			this->user1 = user1 > user2 ? user1 : user2;
			this->user2 = user1 <= user2 ? user1 : user2;
		}
		tags.insert(tag);
	}
};

#endif