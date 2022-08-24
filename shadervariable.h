#pragma once

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>

#define DEFINE_GLM
#ifdef DEFINE_GLM
#include <glm/glm.hpp>
#endif // DEFINE_GLM



void SetVariable(int id,const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

#ifdef DEFINE_GLM
void SetVariable(int id, const std::string& name, const glm::vec2& value)
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void SetVariable(int id, const std::string& name, float x, float y)
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void SetVariable(int id, const std::string& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, const glm::vec4& value)
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void SetVariable(int id, const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void SetVariable(int id, const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
#endif // DEFINE_GLM


template<typename T>
void SetVariable(int id, const std::string& name, const T& value)
{
	static_assert(false, "Unspported Variable Type");
	//std::cout << "Unspported Variable:" + name;
}

class ShaderAble
{
public:
	ShaderAble(const std::string& name) : name(name) {}

	virtual ~ShaderAble() {};

	virtual std::string ShaderName() const { return name; }

	virtual void SetShaderName(const std::string& name){this->name = name;}

	virtual void Write(int id) const = 0;

protected:
	ShaderAble() = default;

	std::string name;
};



class ShaderElement;
class ShaderStructBase;
class ShaderStruct;
template<typename T>
class ShaderArray;

class ShaderElementDataAble : public ShaderAble
{
public:
	ShaderElementDataAble(const std::string& name, ShaderElement* parent = NULL) : ShaderAble(name), parent(parent) {}

	virtual ShaderElementDataAble* Clone() const = 0;

protected:
	ShaderElementDataAble() : parent(NULL) {}

	virtual void WriteInName(int id, const std::string& name) const = 0;

	ShaderElement* parent;
	friend class ShaderElement;
	friend class ShaderArrayElement;
	template<typename T>
	friend class ShaderArray;
};

template<typename T>
class ShaderVariable : public ShaderElementDataAble
{
public:
	ShaderVariable(const std::string& name) : ShaderElementDataAble(name), data(new T) {}
	ShaderVariable(const T& data) : ShaderElementDataAble(""), data(new T(data)) {}
	ShaderVariable(const std::string& name, const T& d)// : ShaderAble(name), data(new T(data)) 
	{
		this->data = new T(d);
	}
	ShaderVariable(const ShaderVariable<T>& other) : data(new T)
	{
		*this = other;
	}

	virtual ~ShaderVariable()
	{
		delete data;
	}

	virtual void Write(int id) const
	{
		WriteInName(id, name);
	}

	virtual ShaderVariable* Clone() const
	{
		return new ShaderVariable(*this);
	}

	virtual ShaderVariable& operator=(const ShaderVariable<T>& other)
	{
		this->name = other.name;
		*this->data = *other.data;
		return *this;
	}

	operator T& ()
	{
		return *data;
	}

protected:
	ShaderVariable() : data(new T) {}

	virtual void WriteInName(int id, const std::string& name) const
	{
		SetVariable(id, name, *data);
	}

	T* data;

	friend class ShaderStructElement;
};

class ShaderElement : public ShaderAble
{
public:
	ShaderElement(const ShaderElement& other) : ShaderAble(other),parent(NULL),eData(NULL)
	{
		*this = other;
	}
	virtual ~ShaderElement()
	{
		if (eData)
		{
			delete eData;
			eData = NULL;
		}
	}

	virtual void Write(int id) const;

	virtual std::string ShaderName() const;
	//{
	//	ShaderElement* se = GetParentElement();
	//	if (!se)
	//	{
	//		return parent->ShaderName() + parent->GetElementString(this);
	//	}
	//	return se->ShaderName() + parent->GetElementString(this);
	//}

	ShaderElement* GetParentElement() const ;

	ShaderElement& operator=(const ShaderElement& other)
	{
		if (eData)
			delete eData;
		eData = NULL;

		if (other.eData)
		{
			eData = other.eData->Clone();
			eData->parent = this;
		}
		return *this;
	}

	template<typename T>
	operator T&()
	{
		ShaderVariable<T>* sv = dynamic_cast<ShaderVariable<T>*>(eData);
		return *sv;
	}

protected:
	ShaderElement(ShaderStructBase* parent, ShaderElementDataAble* eData) : parent(parent),eData(eData)
	{
		if (eData)
			eData->parent = this;
	}

	ShaderStructBase* parent;
	ShaderElementDataAble* eData;

	friend class ShaderStruct;
	template<typename T>
	friend class ShaderArray;
};

class ShaderArrayElement;

class ShaderStructBase : public ShaderElementDataAble
{
public:
	ShaderStructBase(const std::string& name,ShaderElement* parent = NULL) : ShaderElementDataAble(name) {}

protected:
	ShaderStructBase() = default;
	virtual std::string GetElementString(const ShaderElement* e) = 0;

	virtual void WriteInName(int id, const std::string& name) const
	{
		assert(false && "Not implement yet");
	}

	friend class ShaderElement;
};

class ShaderArrayBase : public ShaderStructBase
{
public:
	ShaderArrayBase(const std::string& name, ShaderElement* parent = NULL) : ShaderStructBase(name,parent) {}

	ShaderArrayBase(const std::string& name, const std::initializer_list<ShaderArrayElement>& list, ShaderElement* parent = NULL) : ShaderStructBase(name, parent), _arr(list)
	{

	};

	virtual void Write(int id) const;

	ShaderArrayElement& operator[](size_t i)
	{
		return _arr[i];
	}

	size_t Size() const
	{
		return _arr.size();
	}

protected:
	ShaderArrayBase() = default;

	std::vector<ShaderArrayElement> _arr;
};

class ShaderStructElement : public ShaderElement
{
public:
	template<class T, typename std::enable_if<std::is_base_of<ShaderElementDataAble,T>::value,int>::type = 0>
	ShaderStructElement(T* eData) : ShaderElement(NULL,eData) 
	{
		int a = 1;
	}
	template<class T, typename std::enable_if<std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderStructElement(const T& eData) : ShaderElement(NULL, eData.Clone()) 
	{
		int a = 1;
	}
	template<typename T, typename std::enable_if<!std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderStructElement(const T& eData) : ShaderElement(NULL, new ShaderVariable<T>("",eData)) 
	{
		int a = 1;
	}

	template<typename T, typename std::enable_if<!std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderStructElement& operator=(const T& data)
	{
		ShaderVariable<T>* sv = dynamic_cast<ShaderVariable<T>*>(eData);
		assert(sv);
		*sv->data = data;
		return *this;
	}

protected:
	ShaderStructElement() : ShaderElement(NULL, NULL){}

	friend class ShaderStruct;
};

template<typename T>
class ShaderArray;

class ShaderArrayElement : public ShaderElement
{
public:
	ShaderArrayElement(ShaderStructBase* parent,ShaderElementDataAble* eData) : ShaderElement(parent, eData) {}
	ShaderArrayElement(const ShaderArrayElement& other) : ShaderElement(other){}

	template<class T, typename std::enable_if<std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderArrayElement(T* eData) : ShaderElement(NULL, eData)
	{
		int a = 1;
	}
	template<class T, typename std::enable_if<std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderArrayElement(const T& eData) : ShaderElement(NULL, eData.Clone())
	{
		int a = 1;
	}
	template<typename T, typename std::enable_if<!std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderArrayElement(const T& eData) : ShaderElement(NULL, new ShaderVariable<T>("", eData))
	{
		int a = 1;
	}

	template<typename T, typename std::enable_if<!std::is_base_of<ShaderElementDataAble, T>::value, int>::type = 0>
	ShaderStructElement& operator=(const T& data)
	{
		ShaderVariable<T>* sv = dynamic_cast<ShaderVariable<T>*>(eData);
		assert(sv);
		*sv->data = data;
		return *this;
	}

	ShaderArrayElement& operator[](size_t i)
	{
		ShaderArrayBase* p = dynamic_cast<ShaderArrayBase*>(eData);
		if (!p)
			throw "operator[int] called by non array";

		return p->operator[](i);
	}

	ShaderStructElement& operator[](const std::string& field);

protected:
	ShaderArrayElement() : ShaderElement(NULL, NULL) {}

	template<typename T>
	friend class ShaderArray;
};

class ShaderStruct : public ShaderStructBase
{
public:
	ShaderStruct(const std::string& name, std::initializer_list<std::pair<const std::string, ShaderStructElement> > list = {}) : ShaderStructBase(name), _map(list)
	{
		for (auto it = _map.begin(); it != _map.end(); ++it)
		{
			ShaderStructElement&e = it->second;
			e.eData->SetShaderName(it->first);
			e.parent = this;
		}
	}

	ShaderStruct(const ShaderStruct& prepend, const std::string& name, std::initializer_list<std::pair<const std::string, ShaderStructElement> > list = {}) : ShaderStructBase(name), _map(list)
	{
		for (auto it = _map.begin(); it != _map.end(); ++it)
		{
			ShaderStructElement& e = it->second;
			e.eData->SetShaderName(it->first);
			e.parent = this;
		}

		Insert(prepend);
	}

	virtual void Write(int id) const
	{
		for (auto it = _map.begin(); it != _map.end(); ++it)
		{
			const ShaderStructElement& e = it->second;
			e.Write(id);
		}
	}

	virtual ShaderElementDataAble* Clone() const
	{
		ShaderStruct* clone = new ShaderStruct(*this);
		for (auto it = clone->_map.begin(); it != clone->_map.end(); ++it)
		{
			it->second.parent = clone;
		}
		return clone;
	}

	void Insert(const ShaderStruct& other)
	{
		for (auto it = other._map.begin(); it != other._map.end(); ++it)
		{
			const ShaderStructElement& e = it->second;
			_map.insert(std::make_pair(it->first, e)).first->second.parent = this;
		}
	}

	ShaderStructElement& operator[](const std::string& field)
	{
		return _map.at(field);
	}

protected:
	virtual std::string GetElementString(const ShaderElement* e)
	{
		return '.' + e->eData->ShaderName();
	}

	std::map<std::string, ShaderStructElement> _map;
};

template<typename T>
class ShaderArray : public ShaderArrayBase
{
public:
	ShaderArray(const std::string& name, size_t size, const T& val = T()) : ShaderArrayBase(name)
	{
		Resize(size, val);
	};

	ShaderArray(const std::string& name, const std::initializer_list<ShaderArrayElement>& list) : ShaderArrayBase(name,list)
	{
		for (size_t i = 0; i < _arr.size(); ++i)
		{
			ShaderArrayElement& e = _arr[i];
			e.parent = this;
		}
	};

	virtual ShaderElementDataAble* Clone() const
	{
		ShaderArrayBase* clone = new ShaderArray(*this);
		for (size_t i = 0; i < clone->Size(); ++i)
		{
			ShaderArrayElement& e = clone->operator[](i);
			e.parent = clone;
		}
		return clone;
	}

	void Resize(size_t size, const T& val = T())
	{
		if constexpr (std::is_base_of_v<ShaderElementDataAble, T>)
		{			
			_arr.resize(size, ShaderArrayElement(this, val.Clone()));
			for (size_t i = 0; i < size; ++i)
			{
				ShaderArrayElement& e = _arr[i];
				e.parent = this;
				if (e.eData)
					delete e.eData;
				e.eData = val.Clone();
				e.eData->parent = &e;
			}			
		}
		else
		{
			_arr.resize(size, ShaderArrayElement(this,new ShaderVariable<T>("", val)));
			for (size_t i = 0; i < size; ++i)
			{
				ShaderArrayElement& e = _arr[i];
				e.parent = this;
				if (e.eData)
					delete e.eData;
				e.eData = new ShaderVariable<T>("Unnamed", val);
				e.eData->parent = &e;
			}			
		}
	}
protected:
	virtual std::string GetElementString(const ShaderElement* e)
	{
		size_t index = dynamic_cast<const ShaderArrayElement*>(e) - _arr.data();
		return '[' + std::to_string(index) + ']';
	}
};

std::string ShaderElement::ShaderName() const
{
	ShaderElement* se = GetParentElement();
	if (!se)
	{
		return parent->ShaderName() + parent->GetElementString(this);
	}
	return se->ShaderName() + parent->GetElementString(this);
}

ShaderElement* ShaderElement::GetParentElement() const
{
	return parent->parent;
}

void ShaderElement::Write(int id) const
{
	ShaderStructBase* st = dynamic_cast<ShaderStructBase*>(eData);
	if (st)
		st->Write(id);
	else
	{
		 eData->WriteInName(id, ShaderName());
	}
}

void ShaderArrayBase::Write(int id) const
{
	for (size_t i = 0; i < _arr.size(); ++i)
	{
		_arr[i].Write(id);
	}
}

ShaderStructElement& ShaderArrayElement::operator[](const std::string& field)
{
	ShaderStruct* p = dynamic_cast<ShaderStruct*>(eData);
	if (!p)
		throw "operator[string] called by non struct";

	return p->operator[](field);
}

void ShaderVariableTest()
{	
	////ShaderArray<Test> ttttt;
	ShaderArray<float> array1 ("test", 2);
	ShaderArray<ShaderVariable<float> > array11("test", 1, 1);
	ShaderArray<ShaderArray<float> > array2 ("test", 1, ShaderArray<float>("",1,2));
	//ShaderArray<ShaderArray<ShaderArray<float> > > array3("array3",3,array2);
	//ShaderArray<ShaderArray<ShaderArray<ShaderArray<float> > > >array4("array4", 3, array3);
	ShaderStruct s("s", { {"f1",1},{"f2",2} });
	ShaderArray<ShaderArray<ShaderStruct> > array5("test5", 1, ShaderArray<ShaderStruct>("a",1, s));

	//ShaderStruct struct1("struct1", { {"f1",1},{"f2",new ShaderVariable(1.0f)},{"f3",ShaderVariable(1.0f)} });


	//array1.Resize(2,1.0f);
	//array11.Resize(2, 1.0f);
	////array2.clear();
	//array2.Resize(3, ShaderArray<float>("d",1,1.0f));
	//array2.Resize(4, ShaderArray<float>("d", 1,3));
	//array1[0].SetShaderName("name");
	//array1[1].SetShaderName("name");

	//array2[0][0].SetShaderName("name");
	//array2[1][0].SetShaderName("name");
	//array2[2][0].SetShaderName("name");

	//array3[0][0][0].SetShaderName("name");

	//array4[1][0][0][0].SetShaderName("name");

	std::string s1 = array1[1].ShaderName();
	//auto* st0 = &array2;
	//auto* e0 = array2._arr.data();
	//auto* st1 = dynamic_cast<ShaderArray<float>*>(e0->eData);
	//auto* e1 = st1->_arr.data();
	std::string s2 = array2[0][0].ShaderName();

	ShaderArrayElement& e = array2[0];
	ShaderArrayElement& ee = e[0];
	//std::string s3 = array3[1][0][1].ShaderName();
	//std::string s4 = array4[1][0][0][0].ShaderName();
	////array4[1][0][0][0].Write(1);
	//std::string s5 = struct1.ShaderName();
	//std::string s6 = struct1["f2"].ShaderName();
	std::string s7 = array5[0][0]["f1"].ShaderName();
	int i = array5[0][0]["f1"];
	int a = 0;
}