/*
	Authored 2018, Zhengyang Gu
 
	All rights reserved.

	Redistribution and use in source and binary forms, with or without 
	modification, are permitted provided that the following conditions
	are met:

	*	Redistributions of source code must retain the above
		copyright notice, this list of conditions and the following
		disclaimer.

	*	Redistributions in binary form must reproduce the above
		copyright notice, this list of conditions and the following
		disclaimer in the documentation and/or other materials
		provided with the distribution.

	*	Neither the name of the author nor the names of its
		contributors may be used to endorse or promote products
		derived from this software without specific prior written 
		permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
	POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * This is the data structure used to hold the field mapping of the 
 * ADTs in a Noisy programm, as field names are not stored in struct
 * in LLVM IR.
 * 
 * The "fields" field is an array of strings, which holds the name of
 * the fields in order, e.g.
 * for 
	 * Pixel : adt
	   {
		r : int8;
		g : int8;
		b : int8;
		a : int8;
	   };

 * the array would simply be ["r", "g", "b", "a"]
 */

typedef struct
{
	char *			name;
	char *			fields[];
	int				nFields;
} StructFields;

typedef struct
{
    /*
     * The last register ID we used in the current "scope"
     */
    int             regID;

    /*
     * An array to hold the names of the functions imported,
     * so that we can declare them together.
     */
    char *          loadedFunc[];
    int             nLoadedFunc;

	StructFields *	structs[];
	int				nStructs;

} LlvmBackendState;