#pragma once

#include <cstddef> //ptrdiff_t����

//���ֵ���������
struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };

//Ϊ�˱���������¼traits���˻����Թ��Զ���iterator�̳�֮
template<class Category,class T,class Distance = ptrdiff_t,class Pointer = T*,class Reference =T&>
struct iterator {
	using iterator_category = Category;
	using value_type = T;
	using difference_type = Distance;
	using pointer = Pointer;
	using reference = Reference;
};

//traits չ��Iterator��������
//֮���Բ�ֱ��ʹ�ñ���ģ����ȫ�������Ϊ����ƫ�ػ��汾�����ƺ�����ģ���޷�ƫ�ػ���
template <class Iterator>
struct iterator_traits {
	using iterator_category = typename Iterator::iterator_category;
	using value_type = typename Iterator::value_type;
	using difference_type = typename Iterator::difference_type;
	using pointer = typename Iterator::pointer;
	using reference = typename Iterator::Reference;
};

//���raw pointer��Ƶ�ƫ�ػ��汾
template <class T>
struct iterator_traits<T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using difference_type = ptrdiff_t;
	using pointer = T*;
	using reference = T&;
};

//���pointer-to-const��Ƶ�ƫ�ػ��汾
template <class T>
struct iterator_traits<const T*> {
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using difference_type = ptrdiff_t;
	using pointer = const T* ;
	using reference = const T& ;
};

//����Ϊģ��C++14 type_traits_t���趨�ı���ģ��
template<class T>
using iterator_category_t = typename iterator_traits<T>::iterator_category;

template<class T>
using value_type_t = typename iterator_traits<T>::value_type;

template<class T>
using difference_type_t = typename iterator_traits<T>::difference_type;

template<class T>
using pointer_t = typename iterator_traits<T>::pointer;

template<class T>
using reference_t = typename iterator_traits<T>::reference;

//���������Ի�ȡ����
template<class Iterator>
inline iterator_category_t<Iterator> iterator_category(const Iterator&) {
	return iterator_category_t<Iterator>();
}

template<class Iterator>
inline difference_type_t<Iterator> distance_type(const Iterator&) {
	return  difference_type_t<Iterator>();
}

template<class Iterator>
inline value_type_t<Iterator> value_type(const Iterator&) {
	return  value_type_t<Iterator>();
}