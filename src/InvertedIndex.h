/*
 Build inverted index for the inputfile and ouput the result for( user1,user2,taglist......)
 
 Compiler: g++ version:
 Apple LLVM version 5.1 (clang-503.0.40) (based on LLVM 3.4svn)
 Target: x86_64-apple-darwin13.1.0
 Thread model: posix
 
 From: WangYao, Harbin
 
 */
#ifndef _INVERTEDINDEX_H_
#define _INVERTEDINDEX_H_

#include <iostream>
//in some other compilers the header file may be #include <hash_map> directly
#include <ext/hash_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <set>
#include <list>
#include <fstream>

#include "OutStruct.h"
using namespace std;
#define MAX_COMBINATION 17


class InvertedIndex
{
private:
    //set value for the indexform
	void set_indexform(const string &key, const __int64_t &value)
	{
		indexform_[key].insert(value);
	}
    
	//notice in some other complier the namespace __gnu_cxx may be unnecessary
    //string: store the tag
    //set:store the user list for a certain inverted index
    __gnu_cxx::hash_map<string, set<__int64_t> > indexform_;
    
	//a hashtable to store the final result
    //store information : two related users , the taglist and a flag
	OutStruct outputform_[MAX_COMBINATION];
    
public:
    
    //constructor
	InvertedIndex(const string &filename)
	{
		ifstream fin;
		fin.open(filename);
		InitForm(fin);
		fin.close();
		//out the indexform
		OutIndexform();
        
		cout << "Now is the result for GetIntersection:" << endl;
		vector<__int64_t> tmp_result;
		for (auto k = indexform_.begin(); k != indexform_.end(); ++k)
		{
			//use this to store the intersection of two sets
			for (auto m = indexform_.begin(); m != indexform_.end(); ++m)
			{
				if (k != m)
				{
					tmp_result.clear();
					GetIntersection(k->second.begin(), k->second.end(),
                                    m->second.begin(), m->second.end(), tmp_result);
					if (tmp_result.size() <= 1) continue;
					VectorToOutForm(tmp_result, k->first, m->first);
				}
			}
		}
		OutTheOutputForm();
	}
    //input four iterator and return a vector to store the result of the intersection of two set
    void GetIntersection(set<__int64_t>::iterator first1, const set<__int64_t>::iterator end1,
                         set<__int64_t>::iterator first2, const set<__int64_t>::iterator end2, vector<__int64_t> &result)
	{
		while (first1 != end1 && first2 != end2)
		{
			if (*first1 < *first2) ++first1;
			else if (*first2 < *first1) ++first2;
			else if (*first2 == *first1)
			{
				result.push_back(*first1);
				++first1; ++first2;
			}
		}
	}
    //get the result of two id as the location
	__int64_t UserUnion(const __int64_t &user1,const __int64_t &user2)
	{
		__int64_t ans = user1 & user2;
		return ans;
	}
    //to judge the position in the outputform_ is initialed
	bool FindSpaceInTable(const __int64_t &hash_pair)
	{
		if (outputform_[hash_pair].first == true)
			return false;
		else
			return true;
	}
	inline __int64_t Max(const __int64_t &number1, const __int64_t &number2)
	{
		return number1 > number2 ? number1 : number2;
	}
    
	inline __int64_t Min(const __int64_t &number1, const __int64_t &number2)
	{
		return number1 <= number2 ? number1 : number2;
	}
    
    //to judge whether the id can match or not
	bool IDMatch(const __int64_t &user_id1, const __int64_t &user_id2, const __int64_t &user_pair_id)
	{
		__int64_t big_id = Max( user_id1 , user_id2);
		__int64_t small_id = Min(user_id1, user_id2);
        
		__int64_t hash_user_pair_id = user_pair_id % MAX_COMBINATION;
        
		__int64_t tmp_usr1 = Max(outputform_[hash_user_pair_id].user1, outputform_[hash_user_pair_id].user2);
		__int64_t tmp_usr2 = Min(outputform_[hash_user_pair_id].user1, outputform_[hash_user_pair_id].user2);
        
		if ((tmp_usr1 == big_id) && (tmp_usr2 == small_id))
			return true;
		else
			return false;
	}
    
	__int64_t HashPair(const __int64_t &user_pair_id)
	{
		// get a hash location
		__int64_t ans = user_pair_id % MAX_COMBINATION;
		return ans;
	}
    
    //insert the information into the outputform
	bool InsertTagToPair(const __int64_t &user_id1,const __int64_t &user_id2, const __int64_t &user_pair_id, const string &tag)
	{
		__int64_t hash_pair = 0;
        
		hash_pair = HashPair(user_pair_id);
        
		while (FindSpaceInTable(hash_pair))
		{
			//to get the accurate position of the
			if (IDMatch(user_id1, user_id2, user_pair_id))
			{
				//id match succeess, add tag to the end directly
				outputform_[hash_pair].InsertTag(user_id1, user_id2, tag);
				return true;
			}
			else
			{
                //if id not match move the position backfoward
				++hash_pair;
				hash_pair %= MAX_COMBINATION;
			}
		}
        //find a position which is not used before insert it into the ouputform
		outputform_[hash_pair].InsertTag(user_id1, user_id2, tag);
		return true;
	}
    
	bool VectorToOutForm(const vector<__int64_t> &user_id, const string &tag1, const string &tag2)
	{
		//if the length of the vector
		int vectorsize = user_id.size();
        
		if (vectorsize <= 1) return false;
        
		//get combinations
		__int64_t user_pair_id = 0;
		for (int i = 0; i < vectorsize-1; ++i)
		{
			for (int j = i + 1; j < vectorsize; ++j)
			{
				user_pair_id = UserUnion(user_id[i], user_id[j]);
				InsertTagToPair(user_id[i], user_id[j], user_pair_id, tag1);
				InsertTagToPair(user_id[i], user_id[j], user_pair_id, tag2);
			}
		}
        return true;
	}
    
    //output the inverted indexform in the hash_map
	void OutIndexform()
	{
        cout<<endl;
		for (auto i = indexform_.begin(); i != indexform_.end(); ++i)
		{
			cout << i->first << "\t";
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
				cout << *j << "\t";
            cout<<endl;
		}
		cout << endl;
	}
    
    //output the final result
	void OutTheOutputForm()
	{
        cout<<endl;
		for (int i = 0; i < MAX_COMBINATION; ++i)
		{
			if (outputform_[i].first == false)
			{
				cout << outputform_[i].user1 << "," << outputform_[i].user2<<":\t";
				for (auto j = outputform_[i].tags.begin(); j != outputform_[i].tags.end(); ++j)
					cout<<*j<<"\t";
			}
            else
                continue;
            cout<<endl;
		}
	}
    //build the inverted index form()
	void InitForm(ifstream &fin)
	{
		__int64_t tmp_user = 0;
		string tmp_tags = "";
		// this vector to store the tags of a certain user
		vector<string> tags;
        
		while (!fin.eof())
		{
            //read uid into tmp_user
			fin >> tmp_user;
            //read string to tmp_tags and split it to tmp_string
			getline(fin, tmp_tags);
			istringstream iss(tmp_tags);
			
			string tmp_string = "";
			while (iss >> tmp_string)
			{
				tags.push_back(tmp_string);
			}
            //set value for invertedindex insert tags in the vector into set
			for (auto i = tags.begin(); i != tags.end(); ++i)
			{
				set_indexform(*i, tmp_user);
			}
			tags.clear();
		}
	}
    
};

#endif