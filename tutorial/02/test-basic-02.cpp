/**
 * @FilePath     : /cola/tutorial/02/test-basic-02.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 14:52:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-04 18:36:07
 **/
#include <cassert>

#include "../Common/common.h"
using namespace nao;

void test_timer()
{
    UTimer timer;
    timer.start(1000, [] { NAO_ECHO("Hello, naonao"); });

    NAO_SLEEP_MILLISECOND(5500)
    timer.stop();
}

void test_hash()
{

    const uint8_t data1[1]  = {0x62};
    const uint8_t data2[2]  = {0xc3, 0x97};
    const uint8_t data3[3]  = {0xe2, 0x99, 0xa5};
    const uint8_t data4[4]  = {0xe1, 0x80, 0xb9, 0x32};
    const uint8_t data5[48] = {
        0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18,
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    assert(UHash::Hash(0, 0, 0xbc9f1d34) == 0xbc9f1d34);
    assert(UHash::Hash(reinterpret_cast<const char*>(data1), sizeof(data1), 0xbc9f1d34) == 0xef1345c4);
    assert(UHash::Hash(reinterpret_cast<const char*>(data2), sizeof(data2), 0xbc9f1d34) == 0x5b663814);
    assert(UHash::Hash(reinterpret_cast<const char*>(data3), sizeof(data3), 0xbc9f1d34) == 0x323c078f);
    assert(UHash::Hash(reinterpret_cast<const char*>(data4), sizeof(data4), 0xbc9f1d34) == 0xed21633a);
    assert(UHash::Hash(reinterpret_cast<const char*>(data5), sizeof(data5), 0x12345678) == 0xf333dabb);
}

void test_arean()
{

    std::vector<std::pair<size_t, char*>> allocated;
    UArena                                arena;
    // const int                             N     = 100000;
    const int N     = 100;
    size_t    bytes = 0;
    URandomV2 rnd(301);
    for (int i = 0; i < N; i++) {
        size_t s;
        if (i % (N / 10) == 0) {
            s = i;
        }
        else {
            s = rnd.OneIn(4000) ? rnd.Uniform(6000) : (rnd.OneIn(10) ? rnd.Uniform(100) : rnd.Uniform(20));
        }
        if (s == 0) {
            // Our arena disallows size 0 allocations.
            s = 1;
        }
        char* r;
        if (rnd.OneIn(10)) {
            r = arena.AllocateAligned(s);
        }
        else {
            r = arena.Allocate(s);
        }

        for (size_t b = 0; b < s; b++) {
            // Fill the "i"th allocation with a known bit pattern
            r[b] = i % 256;
        }
        bytes += s;
        allocated.push_back(std::make_pair(s, r));
        std::cout << "arena.MemoryUsage() " << arena.MemoryUsage() << " bytes " << bytes << std::endl;

        if (i > N / 10) {
            std::cout << "arena.MemoryUsage() " << arena.MemoryUsage() << " bytes * 1.10 " << bytes * 1.10 << std::endl;
        }
    }
    for (size_t i = 0; i < allocated.size(); i++) {
        size_t      num_bytes = allocated[i].first;
        const char* p         = allocated[i].second;
        for (size_t b = 0; b < num_bytes; b++) {
            // Check the "i"th allocation for the known bit pattern
            std::cout << "int(p[b]) & 0xff " << (int(p[b]) & 0xff) << " i % 256 " << i % 256 << std::endl;
        }
    }
}

typedef uint64_t Key;
struct Comparator
{
    int operator()(const Key& a, const Key& b) const
    {
        if (a < b) {
            return -1;
        }
        else if (a > b) {
            return +1;
        }
        else {
            return 0;
        }
    }
};
void test_skpilist()
{
    const int                  N = 2000;
    const int                  R = 5000;
    URandomV2                  rnd(1000);
    std::set<Key>              keys;
    UArena                     arena;
    Comparator                 cmp;
    USkipList<Key, Comparator> list(cmp, &arena);
    for (int i = 0; i < N; i++) {
        Key key = rnd.Next() % R;
        if (keys.insert(key).second) {
            list.Insert(key);
        }
    }

    for (int i = 0; i < R; i++) {
        if (list.Contains(i)) {
            assert(keys.count(i) == 1);
        }
        else {
            assert(keys.count(i) == 0);
        }
    }

    // Simple iterator tests
    {
        USkipList<Key, Comparator>::Iterator iter(&list);
        assert(!iter.Valid());

        iter.Seek(0);
        assert(iter.Valid());
        assert(*(keys.begin()) == iter.key());

        iter.SeekToFirst();
        assert(iter.Valid());
        assert(*(keys.begin()) == iter.key());

        iter.SeekToLast();
        assert(iter.Valid());
        assert(*(keys.rbegin()) == iter.key());
    }

    // Forward iteration test
    for (int i = 0; i < R; i++) {
        USkipList<Key, Comparator>::Iterator iter(&list);
        iter.Seek(i);

        // Compare against model iterator
        std::set<Key>::iterator model_iter = keys.lower_bound(i);
        for (int j = 0; j < 3; j++) {
            if (model_iter == keys.end()) {
                assert(!iter.Valid());
                break;
            }
            else {
                assert(iter.Valid());
                assert(*model_iter == iter.key());
                ++model_iter;
                iter.Next();
            }
        }
    }

    // Backward iteration test
    {
        USkipList<Key, Comparator>::Iterator iter(&list);
        iter.SeekToLast();

        // Compare against model iterator
        for (std::set<Key>::reverse_iterator model_iter = keys.rbegin(); model_iter != keys.rend(); ++model_iter) {
            assert(iter.Valid());
            assert(*model_iter == iter.key());
            iter.Prev();
        }
        assert(!iter.Valid());
    }
}

void test_fs()
{
    UFinally fin = []{
        std::cout<<" finally end"<<std::endl;
    };
    std::cout<<" finally start"<<std::endl;
    std::vector<std::string> files;
    files = UFs::getAllFormatFiles(R"(E:\demo\test\nao\str\1.0.0.2)");
}
int main()
{
    // printf("----------------- timer -----------------\n");
    // test_timer();
    // printf("----------------- hash -----------------\n");
    // test_hash();
    // printf("----------------- arean -----------------\n");
    // test_arean();
    //printf("----------------- skpilist -----------------\n");
    //test_skpilist();

    printf("----------------- fs -----------------\n");
    test_fs();
    return 0;
}