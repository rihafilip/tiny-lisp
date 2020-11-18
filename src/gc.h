#pragma once

#include "instruction.h"
#include <string>
#include <utility>

namespace lisp
{
	/**
	 * Global garbage collector, allocates space for value
	 * Works as simple mark-and-sweep gc
	 */
	class GC
	{
	private:
		/// Default ammount of memory block that will be avaliable
		static const uint LIST_SIZE = 100;
	public:
		/**
		 * Type of raw data
		 */
		enum MemoryType
		{
			UNDEF, /// not defined
			EMPTY, /// empty (null) memory block
			NUM, /// integer
			SYM, /// symbol
			CONS, /// cons cell
			INST /// SEDC instruction
		};

		/**
		 * Raw data blocks
		 */
		struct Memory
		{
		private:
			bool marked;
			friend class GC;

			/// Marks this cell and all of it correspoding cells as marked
			void Mark();

		public:
			/// Default constructor
			Memory();
			/// Destructor, takes care of name
			~Memory();
			/// equality comparator
			bool operator== ( const Memory & other );

			/// Type of memory cell this is
			MemoryType type;
			union
			{
				int number;
				// symbol name
				const char * name;
				// car, cdr
				std::pair< Memory *, Memory *> cons;
				sedc::Instruction instruct;
			};
		};

		/// Starts the garbage collector, initializes all needed parameters
		static void Start( int howMuch = LIST_SIZE );
		/// Allocates memory buffers
		static void Allocate ( int howMuch = LIST_SIZE );
		// Stops the garbage collector and prevents memory leaks
		static void Stop();
		/// Adds a Memory block to roots
		static void AddRoot ( Memory * root );
		/// Removes a memory block from roots
		static void RemoveRoot( Memory * root );

		/**
		 * \defgroup GetMemory Get allocated memory of ceraint type
		 * @{
		 */
		static Memory * GetMemory( int num );
		static Memory * GetMemory( const std::string & str );
		static Memory * GetMemory( Memory * car, Memory * cdr );
		static Memory * GetMemory ( sedc::Instruction instruct);
		static Memory * GetNull ();
		/// @}

	private:
		/**
		 * Simple structure for holding memory blocks
		 */
		struct List
		{
			List ( Memory * inMem, List * inNext = nullptr )
			: memory (inMem)
			, next (inNext)
			{}

			~List()
			{ delete next; }

			/// Cleans memory blocks
			void clean ()
			{
				delete memory;
				if ( next != nullptr )
					next -> clean();
			}

			Memory * memory;
			List * next;
		};

		/// List of roots
		static List * m_Roots;
		/// List of available memory
		static List * m_Available;
		/// List of all memory blocks
		static List * m_All;

		/// Pointer to single null memory representator
		static Memory * m_Null;

		/// \defgroup GC_proc Garbage collection procedures
		/// @{
		static void Collect ();
		static void Mark ( List * lst = m_Roots );
		static void Sweep ( List * lst = m_All );
		/// @}
		
		/// Finds memory block in list and deletes it
		static void RemoveRoot( Memory * mem, List * current, List * prev = nullptr );

		/// Return the next available memory block
		static Memory * GetNextEmptyMemory ();
	};

	std::string toString ( GC::MemoryType type );
}
