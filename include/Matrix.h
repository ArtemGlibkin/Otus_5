#include <list>
#include <functional>

/*!
	\brief Класс для хранения разреженных матриц
	\author Glibkin Artyom
*/
template<typename T, T default_val>
class Matrix
{
	using deleter_func = std::function<void()>;

	/*!
	\brief Класс для хранения элемента матрицы
	*/
	template<typename U>
	struct Node
	{
		int row;
		int column;
		T value;


		Node(int row, int column, const T& value) : row(row), column(column), value(value) {}
		
		Node(const Node& node)
		{
			value = node.value;
			row = node.row;
			column = node.column;
			if (node.value == default_val)
				node.deleter();
		}

		Node(Node&& node)
		{
			value = node.value;
			row = node.row;
			column = node.column;
			deleter = node.deleter;
		}
		
		void set_deleter(deleter_func func)
		{
			deleter = func;
		}
		
		operator std::tuple <int&, int&, U&>() {
			return std::make_tuple(std::ref(row), std::ref(column), std::ref(value));
		}

		bool operator==(const U& val)
		{
			return value == val;
		}

		void operator=(const U& val) {
			if (val == default_val)
			{
				delete_node();
				return;
			}
			value = val;
		}
	private:
		deleter_func deleter = nullptr;

		void delete_node()
		{
			if (deleter != nullptr)
				deleter();
		}
	};

	/*!
	\brief Класс для хранения строки матрицы
	*/
	template<typename U>
	struct Row
	{
		int row;
		std::list<Node<U>> nodes;
		deleter_func deleter;

		Row(int r) : row(r) {}
		size_t size()
		{
			return nodes.size();
		}
		size_t columns()
		{
			return nodes.back().column;
		}

		void print(int columns)
		{
			auto last_it = nodes.begin();
			print_empty_columns(last_it->column);
			for (auto it = last_it; it != nodes.end(); it++)
			{
				print_empty_columns(it->column - last_it->column - 1);

				std::cout << it->value << " ";
				last_it = it;
			}
			print_empty_columns(columns - last_it->column);
			std::cout << std::endl;
		}

		void set_deleter(const deleter_func& del)
		{
			deleter = del;
		}


		Node<U>& operator[](int column)
		{
			auto it = nodes.begin();
			for (it; it != nodes.end(); it++)
			{
				if (it->column == column)
					return *it;
				else if (it->column > column)
				{
					return *addColumn(it, row, column, default_val);
				}
			}
			return *addColumn(it, row, column, default_val);
		}
	private:

		using iterator = decltype(nodes.begin());
		iterator addColumn(iterator it, int row, int column, U&& value)
		{
			it = nodes.insert(it, Node<T>(row, column, value));
			it->set_deleter(
				[&, it]()
				{
					if (nodes.size() == 1)
						delete_row();
					else
						nodes.erase(it);
				}
			);
			return it;
		}

		void print_empty_columns(int n)
		{
			for (int i = 0; i < n; i++)
				std::cout << default_val << " ";
		}

		void delete_row()
		{
			if (deleter != nullptr)
				deleter();
		}
	};

	std::list<Row<T>> rows;

	using row_iterator = decltype(rows.begin());
	using column_iterator = decltype(rows.begin()->nodes.begin());
	struct Iterator
	{
		Iterator(const row_iterator& ptr, const row_iterator& ptr_end, const column_iterator& cptr) : r_ptr(ptr), r_ptr_end(ptr_end), c_ptr(cptr)
		{
		
		};

		Node<T>& operator*() const { return *c_ptr; }
		Node<T>* operator->() { return &(*c_ptr); }
		
		Iterator& operator++() {
			c_ptr++;
			if (c_ptr == r_ptr->nodes.end())
			{
				r_ptr++;
				if(r_ptr != r_ptr_end)
					c_ptr = r_ptr->nodes.begin();
			}
			return *this; 
		}

		friend bool operator== (const Iterator& a, const Iterator& b) {
			return a.c_ptr == b.c_ptr;
		}
		friend bool operator!= (const Iterator& a, const Iterator& b) {
			return (a.c_ptr != b.c_ptr);
		}
	private:
		row_iterator r_ptr;
		row_iterator r_ptr_end;
		column_iterator c_ptr;
		friend class Matrix;
		bool perevod = false;
	};

	/*!
			Возвращает максимальное количество строков в матрице
			\return количество столбцов в матрице
	*/
	size_t getMaxColumns()
	{
		size_t max = 0;
		for (auto it : rows)
		{
			auto row_max = it.columns();
			if (row_max > max)
				max = row_max;
		}
		return max;
	}

	using iterator = decltype(rows.begin());

	/*!
			Добавляет строку с заданным номером перед итератором
			\param it итератор
			\param row номер строки
			\return итератор на новую строку
	*/
	iterator addRow(iterator it, int row)
	{
		it = rows.insert(it, Row<T>(row));
		it->set_deleter([&, it]() {rows.erase(it); });
		return it;
	}

	/*!
			Печать n строк с значением по умолчанию
			\param n количество строк
			\param columns количество столбцов
	*/

	void print_empty_rows(int n, int columns)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j <= columns; j++)
			{
				std::cout << default_val << " ";
			}
			std::cout << std::endl;
		}
	}

public:

	/*!
			Возвращает итератор на начало матрицы
	*/
	Iterator begin()
	{
		auto it = rows.begin();
		return Iterator(it, rows.end(), it->nodes.begin());
	}

	/*!
			Возвращает итератор на конец матрицы
	*/
	Iterator end()
	{
		auto it = --rows.end();
		return Iterator(it, rows.end(), it->nodes.end());
	}

	Row<T>& operator[](int row)
	{
		auto it = rows.begin();
		for (it; it != rows.end(); it++)
		{
			if (it->row == row)
				return *it;
			else if (it->row > row)
				return *addRow(it, row);
		}
		return *addRow(it, row);
	}

	/*!
			Возвращает количество хранимых элементов
			\return размер
	*/
	size_t size()
	{
		size_t size = 0;
		for (auto it : rows)
			size += it.size();
		return size;
	}

	/*!
			Печать матрицы
			\param i строка с которой начнется вывод
			\param i_max строка на которой закончится вывод
			\param j_max количество выводимых столбцов
	*/

	void print(int i = 0, int i_max = -1, int j_max = -1)
	{
		auto pred = rows.begin();
		size_t max_columns = j_max == -1 ? getMaxColumns() : j_max;
		size_t max_row = i_max == -1 ? rows.back().row : i_max;
		print_empty_rows(pred->row - i, max_columns);

		for (auto it = pred; it != rows.end(); it++)
		{
			if (it->row < i)
				continue;

			if (it->row > max_row)
				return;

			print_empty_rows(it->row - (pred->row < i ? i : pred->row) -1, max_columns);
			it->print(max_columns);
			pred = it;
		}

		print_empty_rows(max_row - pred->row - 1, max_columns);
	}
};