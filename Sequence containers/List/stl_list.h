#pragma once
#include "stl_list_iterator.h"

template<class T,class Alloc=alloc>//Ĭ����allocΪ�ռ�������
class list {//˫��ѭ������
protected:
	using list_node = __list_node<T>;
	//ר����������ÿ������һ���ڵ�Ĵ�С
	using list_node_allocator = simple_alloc<list_node, Alloc>;
public:
	using link_type = list_node;
	using iterator = __list_iterator::iterator;
	using reference = __list_iterator::reference;
	using size_type = __list_iterator::size_type;
protected:
	//�������ͷ�node
	link_type get_node() { return list_node_allocator::alloc(); }
	void put_node(link_type p) { list_node_allocator::deallocate(p); }
	//����������node
	link_type create_node(const T& value) {
		link_type p = get_node();
		construct(&p->data, value);
		return p;
	}
	void destory_node(link_type p) {
		destory(&p->data);
		put_node(p);
	}
protected:
	list_node node;
protected:
	//����һ����list
	void empety_initialized() {
		node = get_node();
		node->next = node;
		node->prev = node;
	}
	//��ָ��λ��֮ǰ����value
	iterator insert(iterator position, const T&value) {
		link_type temp = create_node(value);
		temp->next = position.node;
		temp->prev = position.node->prev;
		static_cast<link_type>(position.node->prev)->next = temp;
		position.node->prev = temp;
		return temp;
	}
	//ɾ��ָ��λ�õĽڵ�
	iterator erase(iterator position) {
		//����ֲ������Ƿ�Ӱ��Ч�ʣ�
		link_type next_node = static_cast<link_type>(position.node->next);
		link_type prev_node = static_cast<link_type>(position.node->prev);
		prev_node->next = next_node;
		next_node->prev = prev_node;
		destory_node(position.node);
		return static_cast<iterator>(next_node);
	}
	//��[first,last)�����ƶ���pos֮ǰ
	void transfer(iterator position, iterator first, iterator last) {
		if (position != last) {
			//��������
			static_cast<link_type>(last.node->prev)->next = position.node;
			static_cast<link_type>(first.node->prev)->next = last.node;
			//�������
			static_cast<link_type>(position.node->prev)->next = first.node;
			link_type temp = position.node->prev;
			position.node->prev = last.node->prev;
			last.node->prev = first.node->prev;
			first.node->prev = temp;
		}
	}

public:
	list() { empety_initialized(); }
public:
	//��̬�ӿ�
	//����nodeΪĩ�ڵ�(ĩ�ڵ��ƺ�Ϊ�ڱ���������������ӿ�ʱ�临�Ӷ�ΪO(1)
	iterator begin() { return static_cast<link_type>(node->next); }
	iterator end() { return node; }
	bool empty() const { return node->next == node; }
	reference front() { return *begin(); }
	reference back()  { return *end(); }
	
	size_type size() const {
		size_type result = 0;
		distance(begin(), end(), result);//ȫ�ֺ�������stl_iterator.h
		return result;
	}
public:
	//��̬�ӿ�
	void push_front(const T&value) { insert(begin(), value); }
	void push_back(const T&value) { insert(end(), value); }
	void pop_fornt() { erase(begin()); }
	void pop_back() {
		iterator temp = end();
		erase(--temp);
	}
	void clear();
	void remove(const T& value);
	//����ȥ��
	void unique();
	//��x�ƶ���pos֮ǰ,x���벻ͬ��*this
	void splice(iterator position, list& x);
	//��i��ָ��Ԫ���ƶ���pos֮ǰ��pos��i����ָ��ͬһ��list
	void splice(iterator position, list&, iterator i);
	//�����������list�鲢
	void merge(list& x);
	void reverse();
	void sort();
};

template<class T, class Alloc>
inline void list<T, Alloc>::clear(){
	link_type cur = static_cast<link_type>(node->next);//begin()
	while (cur != node) {
		link_type temp = cur;
		cur = static_cast<link_type>(cur->next);
		destory_node(temp);
	}
	//�ָ�ԭʼ״̬
	node->next = node;
	node->prev = node;
}

template<class T, class Alloc>
inline void list<T, Alloc>::remove(const T& value){
	iterator first = begin();
	iterator end = end();
	while (first != end) {
		iterator next = first;
		++next;
		if (*first == value) erase(first);
		first = next;
	}
}

template<class T, class Alloc>
inline void list<T, Alloc>::unique(){
	iterator first = begin();
	iterator end = end();
	if (first == end) return;
	iterator next = first;
	while (++next != end) {
		if (*first == *next) {
			erase(next);
			next = first;//����next
		}
		else
			first = next;
	}
}

template<class T, class Alloc>
inline void list<T, Alloc>::splice(iterator position, list& x){
	if (!x.empty()) 
		transfer(position, x.begin(), x.end();
}

template<class T, class Alloc>
inline void list<T, Alloc>::splice(iterator position, list &, iterator i){
	iterator j = i;
	++j;
	//i==pos �����޷���������֮ǰ
	//j==pos �Ѵ���pos֮ǰ
	if (position == i || position == j) return;
	transfer(position, i, j);
}

template<class T, class Alloc>
inline void list<T, Alloc>::merge(list& x){
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();

	while (first1 != last1 && first2 != last2) {
		if (*first2 < *first1) {
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	}
	if (first2 != last2) transfer(last1, first2, last2);
}

template<class T, class Alloc>
inline void list<T, Alloc>::reverse(){
	//��list�����һ��Ԫ��
	if (node->next == node || node->next->next == node) return;
	iterator first = begin();
	++first;//begin��������Ҫ�ƶ���������ΪָʾĩԪ�ص��ڱ���ȷ�е�˵������begin.node->next == end.node)
	while (first != end()) {
		iterator old = first;
		++first;
		transfer(begin(), old, first);
	}
}

//STL list����ʹ��STL sort�㷨��������ҪrandomAccess������
//��member function����quick sort��Ȼ���Ҹо������ڹ鲢����
//�㷨���ܼ� https://blog.csdn.net/qq276592716/article/details/7932483
template<class T, class Alloc>
inline void list<T, Alloc>::sort(){
	if (node->next == node || node->next->next == node) return;
	//�н����ݴ���� counter[n]�������2^(n+1)��Ԫ�أ�����������counter[n+1]���鲢
	list carry;
	list counter[64];
	int fill = 0;
	while (!empty()) {
		carry.splice(carry.begin(), *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty()) {
			counter[i].merge(carry);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if (i == fill) 
			++fill;
	}
	for (int i = 1; i < fill; ++i)
		counter[i].merge(counter[i - 1]);
	swap(counter[fill - 1]);
}
