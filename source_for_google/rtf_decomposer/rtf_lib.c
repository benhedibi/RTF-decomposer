#include "rtf_lib.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "hex_tables.h"

#include "common.h"

#define RTF_SIGNATURE (0x74725C7B ^ MAXDWORD) // like microsoft word "{/rt"
#define IS_VALID_RTF_SIGNATURE(x) (((*(puint32)(x)) ^ MAXDWORD) == RTF_SIGNATURE)

typedef struct {
	             puchar name;
				 unsigned_int name_len;				 
               } t_rtf_control_name,*p_rtf_control_name;

typedef enum {
	           t_undefined_rtf_tag,
	           t_rtf_object_tag,
	           t_rtf_object_data_tag,
	           t_rtf_font_tag,
	           t_rtf_picture_tag,
	           t_rtf_datafield_tag,
			   t_rtf_datastore_tag,
			   t_rtf_shape_tag,
	           MAX_RTF_TAG = t_rtf_shape_tag
             } t_rtf_tag;


typedef puchar t_control_callback(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data);
typedef t_control_callback *p_control_callback;

static puchar consume_data(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data);
static puchar read_bin_control(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data);
static puchar execute_sv_control(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data);


typedef struct {
	             puchar name;
	             unsigned_int len;
	             p_control_callback callback;
               } t_tag_definition, *p_tag_definition;


static const t_tag_definition a_controls[] = { { "aftncn",6,consume_data },{ "aftnsep",7,consume_data },{ "aftnsepc",8,consume_data },{ "annotation",10,consume_data },
{ "atnauthor",9,consume_data },{ "atndate",7,consume_data },{ "atnid",5,consume_data },{ "atnparent",9,consume_data },
{ "atnref",6,consume_data },{ "atrfend",7,consume_data },{ "atrfstart",9,consume_data },{ "author",6,consume_data } };

static const t_tag_definition b_controls[] = { { "background",10,consume_data },{ "bin",3,read_bin_control },{ "bkmkend",7,consume_data },{ "bkmkstart",9,consume_data },
{ "blipuid",7,consume_data },{ "buptim",6,consume_data } };


static const t_tag_definition c_controls[] = { { "category",8,consume_data },{ "colorschememapping",18,consume_data },{ "colortbl",8,consume_data },{ "comment",7,consume_data },
{ "company",7,consume_data },{ "creatim",7,consume_data } };

static const t_tag_definition d_controls[] = { { "datafield",9,consume_data },{ "datastore",9,consume_data },{ "defchp",6,consume_data },{ "defpap",6,consume_data },{ "do",2,consume_data },
{ "doccomm",7,consume_data },{ "docvar",6,consume_data },{ "dptxbxtext",10,consume_data } };

static const t_tag_definition e_controls[] = { { "ebcend",6,consume_data },{ "ebcstart",8,consume_data } };


static const t_tag_definition f_controls[] = { { "factoidname",11,consume_data },{ "falt",4,consume_data },{ "fchars",6,consume_data },{ "ffdeftext",9,consume_data },
{ "ffentrymcr",10,consume_data },{ "ffexitmcr",9,consume_data },{ "ffformat",8,consume_data },{ "ffhelptext",10,consume_data },
{ "ffl",3,consume_data },{ "ffname",6,consume_data },{ "ffstattext",10,consume_data },{ "field",5,consume_data },{ "file",4,consume_data },
{ "filetbl",7,consume_data },{ "fldinst",7,consume_data },{ "fldrslt",7,consume_data },{ "fldtype",7,consume_data },
{ "fontemb",7,consume_data },{ "fonttbl",7,consume_data },{ "footer",6,consume_data },{ "footerf",7,consume_data },{ "footerl",7,consume_data },
{ "footerr",7,consume_data },{ "footnote",8,consume_data },{ "formfield",9,consume_data },{ "ftncn",5,consume_data },{ "ftnsep",6,consume_data },
{ "ftnsepc",7,consume_data } };


static const t_tag_definition g_controls[] = { { "g",1,consume_data },{ "generator",9,consume_data },{ "gridtbl",7,consume_data } };


static const t_tag_definition h_controls[] = { { "header",6,consume_data },{ "headerf",7,consume_data },{ "headerl",7,consume_data },{ "headerr",7,consume_data },{ "hl",2,consume_data },
{ "hlfr",4,consume_data },{ "hlinkbase",9,consume_data },{ "hlloc",5,consume_data },{ "hlsrc",5,consume_data },{ "hsv",3,consume_data } };


static const t_tag_definition i_controls[] = { { "info",4,consume_data } };


static const t_tag_definition k_controls[] = { { "keywords",8,consume_data } };


static const t_tag_definition l_controls[] = { { "latentstyles",12,consume_data },{ "lchars",6,consume_data },{ "levelnumbers",12,consume_data },{ "leveltext",9,consume_data },
{ "lfolevel",8,consume_data },{ "linkval",7,consume_data },{ "list",4,consume_data },{ "listlevel",9,consume_data },
{ "listname",8,consume_data },{ "listoverride",12,consume_data },{ "listoverridetable",17,consume_data },{ "listpicture",11,consume_data },
{ "liststylename",13,consume_data },{ "listtable",9,consume_data },{ "listtext",8,consume_data },{ "lsdlockedexcept",15,consume_data } };


static const t_tag_definition m_controls[] = { { "macc",4,consume_data },{ "maccPr",6,consume_data },{ "mailmerge",9,consume_data },{ "malnScr",7,consume_data },{ "manager",7,consume_data },
{ "margPr",6,consume_data },{ "margSz",6,consume_data },{ "mbar",4,consume_data },{ "mbarPr",6,consume_data },{ "mbaseJc",7,consume_data },{ "mbegChr",7,consume_data },
{ "mborderBox",10,consume_data },{ "mborderBoxPr",12,consume_data },{ "mbox",4,consume_data },{ "mboxPr",6,consume_data },
{ "mchr",4,consume_data },{ "mcount",6,consume_data },{ "mctrlPr",7,consume_data },{ "md",2,consume_data },{ "mdeg",4,consume_data },
{ "mdegHide",8,consume_data },{ "mden",4,consume_data },{ "mdiff",5,consume_data },{ "mdPr",4,consume_data },{ "me",2,consume_data },
{ "mendChr",7,consume_data },{ "meqArr",6,consume_data },{ "meqArrPr",8,consume_data },{ "mf",2,consume_data },{ "mfName",6,consume_data },
{ "mfPr",4,consume_data },{ "mfunc",5,consume_data },{ "mfuncPr",7,consume_data },{ "mgroupChr",9,consume_data },{ "mgroupChrPr",11,consume_data },
{ "mgrow",5,consume_data },{ "mhideBot",8,consume_data },{ "mhideLeft",9,consume_data },{ "mhideRight",10,consume_data },
{ "mhideTop",8,consume_data },{ "mlim",4,consume_data },{ "mlimLoc",7,consume_data },{ "mlimLow",7,consume_data },{ "mlimLowPr",9,consume_data },
{ "mlimUpp",7,consume_data },{ "mlimUppPr",9,consume_data },{ "mm",2,consume_data },{ "mmaddfieldname",14,consume_data },{ "mmathPict",9,consume_data },
{ "mmaxDist",8,consume_data },{ "mmc",3,consume_data },{ "mmcJc",5,consume_data },{ "mmconnectstr",12,consume_data },{ "mmconnectstrdata",16,consume_data },
{ "mmcPr",5,consume_data },{ "mmcs",4,consume_data },{ "mmdatasource",12,consume_data },{ "mmheadersource",14,consume_data },
{ "mmmailsubject",13,consume_data },{ "mmodso",6,consume_data },{ "mmodsofilter",12,consume_data },{ "mmodsofldmpdata",15,consume_data },
{ "mmodsomappedname",16,consume_data },{ "mmodsoname",10,consume_data },{ "mmodsorecipdata",15,consume_data },{ "mmodsosort",10,consume_data },
{ "mmodsosrc",9,consume_data },{ "mmodsotable",11,consume_data },{ "mmodsoudl",9,consume_data },{ "mmodsoudldata",13,consume_data },
{ "mmodsouniquetag",15,consume_data },{ "mmPr",4,consume_data },{ "mmquery",7,consume_data },{ "mmr",3,consume_data },{ "mnary",5,consume_data },
{ "mnaryPr",7,consume_data },{ "mnoBreak",8,consume_data },{ "mnum",4,consume_data },{ "mobjDist",8,consume_data },{ "moMath",6,consume_data },
{ "moMathPara",10,consume_data },{ "moMathParaPr",12,consume_data },{ "mopEmu",6,consume_data },{ "mphant",6,consume_data },{ "mphantPr",8,consume_data },
{ "mplcHide",8,consume_data },{ "mpos",4,consume_data },{ "mr",2,consume_data },{ "mrad",4,consume_data },{ "mradPr",6,consume_data },{ "mrPr",4,consume_data },
{ "msepChr",7,consume_data },{ "mshow",5,consume_data },{ "mshp",4,consume_data },{ "msPre",5,consume_data },{ "msPrePr",7,consume_data },{ "msSub",5,consume_data },
{ "msSubPr",7,consume_data },{ "msSubSup",8,consume_data },{ "msSubSupPr",10,consume_data },{ "msSup",5,consume_data },{ "msSupPr",7,consume_data },
{ "mstrikeBLTR",11,consume_data },{ "mstrikeH",8,consume_data },{ "mstrikeTLBR",11,consume_data },{ "mstrikeV",8,consume_data },{ "msub",4,consume_data },
{ "msubHide",8,consume_data },{ "msup",4,consume_data },{ "msupHide",8,consume_data },{ "mtransp",7,consume_data },{ "mtype",5,consume_data },{ "mvertJc",7,consume_data },
{ "mvfmf",5,consume_data },{ "mvfml",5,consume_data },{ "mvtof",5,consume_data },{ "mvtol",5,consume_data },{ "mzeroAsc",8,consume_data },{ "mzeroDesc",9,consume_data },
{ "mzeroWid",8,consume_data } };


static const t_tag_definition n_controls[] = { { "nesttableprops",14,consume_data },{ "nonesttables",12,consume_data } };

// {"objclass",8},{"objdata",7},{"object",6},{"objname",7}
static const t_tag_definition o_controls[] = { { "objalias",8,consume_data },{ "objsect",7,consume_data },{ "oldcprops",9,consume_data },{ "oldpprops",9,consume_data },{ "oldsprops",9,consume_data },
{ "oldtprops",9,consume_data },{ "oleclsid",8,consume_data },{ "operator",8,consume_data } };


static const t_tag_definition p_controls[] = { { "panose",6,consume_data },{ "password",8,consume_data },{ "passwordhash",12,consume_data },{ "pgp",3,consume_data },
{ "pgptbl",6,consume_data },{ "picprop",7,consume_data },{ "pict",4,consume_data },{ "pn",2,consume_data },{ "pnseclvl",8,consume_data },
{ "pntext",6,consume_data },{ "pntxta",6,consume_data },{ "pntxtb",6,consume_data },{ "printim",7,consume_data },{ "propname",8,consume_data },
{ "protend",7,consume_data },{ "protstart",9,consume_data },{ "protusertbl",11,consume_data } };


static const t_tag_definition r_controls[] = { { "result",6,consume_data },{ "revtbl",6,consume_data },{ "revtim",6,consume_data },{ "rxe",3,consume_data } };


static const t_tag_definition s_controls[] = { { "shp",3,consume_data },{ "shpgrp",6,consume_data },{ "shpinst",7,consume_data },{ "shppict",7,consume_data },{ "shprslt",7,consume_data },
{ "shptxt",6,consume_data },{ "sn",2,consume_data },{ "sp",2,consume_data },{ "staticval",9,consume_data },{ "stylesheet",10,consume_data },
{ "subject",7,consume_data },{ "sv",2,consume_data/*execute_sv_control*/ },{ "svb",3,consume_data } };


static const t_tag_definition t_controls[] = { { "tc",2,consume_data },{ "template",8,consume_data },{ "themedata",9,consume_data },{ "title",5,consume_data },{ "txe",3,consume_data } };


static const t_tag_definition u_controls[] = { { "ud",2,consume_data },{ "upr",3,consume_data },{ "userprops",9,consume_data } };


static const t_tag_definition w_controls[] = { { "wgrffmtfilter",13,consume_data },{ "windowcaption",13,consume_data },{ "writereservation",16,consume_data },{ "writereservhash",15,consume_data } };


static const t_tag_definition x_controls[] = { { "xe",2,consume_data },{ "xform",5,consume_data },{ "xmlattrname",11,consume_data },{ "xmlattrvalue",12,consume_data },{ "xmlclose",8,consume_data },
{ "xmlname",7,consume_data },{ "xmlnstbl",8,consume_data },{ "xmlopen",7,consume_data } };


typedef struct {
	             unsigned_int tags_count;
	             p_tag_definition tag_defs;
               } t_tag_def_table_entry, *p_tag_def_table_entry;


static const t_tag_def_table_entry table_a = { { sizeof(a_controls) / sizeof(a_controls[0]) },{ &a_controls } };
static const t_tag_def_table_entry table_b = { { sizeof(b_controls) / sizeof(b_controls[0]) },{ &b_controls } };
static const t_tag_def_table_entry table_c = { { sizeof(c_controls) / sizeof(c_controls[0]) },{ &c_controls } };
static const t_tag_def_table_entry table_d = { { sizeof(d_controls) / sizeof(d_controls[0]) },{ &d_controls } };
static const t_tag_def_table_entry table_e = { { sizeof(e_controls) / sizeof(e_controls[0]) },{ &e_controls } };
static const t_tag_def_table_entry table_f = { { sizeof(f_controls) / sizeof(f_controls[0]) },{ &f_controls } };
static const t_tag_def_table_entry table_g = { { sizeof(g_controls) / sizeof(g_controls[0]) },{ &g_controls } };
static const t_tag_def_table_entry table_h = { { sizeof(h_controls) / sizeof(h_controls[0]) },{ &h_controls } };
static const t_tag_def_table_entry table_i = { { sizeof(i_controls) / sizeof(i_controls[0]) },{ &i_controls } };
static const t_tag_def_table_entry table_k = { { sizeof(k_controls) / sizeof(k_controls[0]) },{ &k_controls } };
static const t_tag_def_table_entry table_l = { { sizeof(l_controls) / sizeof(l_controls[0]) },{ &l_controls } };
static const t_tag_def_table_entry table_m = { { sizeof(m_controls) / sizeof(m_controls[0]) },{ &m_controls } };
static const t_tag_def_table_entry table_n = { { sizeof(n_controls) / sizeof(n_controls[0]) },{ &n_controls } };
static const t_tag_def_table_entry table_o = { { sizeof(o_controls) / sizeof(o_controls[0]) },{ &o_controls } };
static const t_tag_def_table_entry table_p = { { sizeof(p_controls) / sizeof(p_controls[0]) },{ &p_controls } };
static const t_tag_def_table_entry table_r = { { sizeof(r_controls) / sizeof(r_controls[0]) },{ &r_controls } };
static const t_tag_def_table_entry table_s = { { sizeof(s_controls) / sizeof(s_controls[0]) },{ &s_controls } };
static const t_tag_def_table_entry table_t = { { sizeof(t_controls) / sizeof(t_controls[0]) },{ &t_controls } };
static const t_tag_def_table_entry table_u = { { sizeof(u_controls) / sizeof(u_controls[0]) },{ &u_controls } };
static const t_tag_def_table_entry table_w = { { sizeof(w_controls) / sizeof(w_controls[0]) },{ &w_controls } };
static const t_tag_def_table_entry table_x = { { sizeof(x_controls) / sizeof(x_controls[0]) },{ &x_controls } };

static const p_tag_def_table_entry tag_defs_lookup[] = { &table_a,&table_b,&table_c,&table_d,&table_e,&table_f,&table_g,&table_h,&table_i,NULL,&table_k,&table_l,&table_m,
&table_n,&table_o,&table_p,NULL,&table_r,&table_s,&table_t,&table_u,NULL,&table_w,&table_x,NULL,NULL };


typedef struct {
	             puchar ctrl_name;
	             unsigned_int ctrl_name_len;
	             puchar ctrl_param;
	             unsigned_int ctrl_param_len;
               } t_rtf_cword_infos, *p_rtf_cword_infos;


static byte hex_to_byte(const puchar hex)
{
  uint8 a, b;

	a = unhex_table[*hex];
    b = unhex_table[*(hex + 1)];
   
    return (a << 4) | b;
}

static uint32 hex_to_dword(const puchar hex)
{
  uint64 n, alphahex,n0,n1,n2;
  uint32 result;
 
    n = (*(puint64)(hex)) & 0x4F4F4F4F4F4F4F4Full;

	alphahex = (uint64)(n & 0x4040404040404040ull);
	
	n0 = alphahex == 0 ? n : ((alphahex >> 6) * 9) + (n ^ alphahex);
	n1 = n0 * 0x1001 >> 8;
	n2 = (n1 & 0x00FF00FF00FF00FFull) * 0x1000001 >> 16;
	
	uint32 num = (n2 & 0x0000FFFF0000FFFFull) * 0x1000000000001 >> 32;

	return BSWAP(num);
}

boolean open_rtf_strm(const puchar buffer, const unsigned_int buffer_size, const p_rtf_strm strm)
{

	if ((!buffer) || ((signed_int)buffer_size <= 0) || (!strm))
	{
		return false;
	}
	
	if ((buffer_size <= 4) || (!IS_VALID_RTF_SIGNATURE(buffer)))
	{
		strm->error_code = RTF_INVALID_SIGNATURE_ERROR;
		return false;
	}

	strm->malformed_document += (unsigned_int)(buffer[5] - 'f');
	
	strm->data = buffer;
	strm->data_size = buffer_size;
	
	strm->error_code = ERROR_SUCCESS;
	
	strm->hdr_flags = 0;
	
	strm->last_state = t_rtf_undefined_state;

	strm->malformed_document = 0;

	strm->overlay_data = NULL;
	strm->overlay_size = 0;

	if (!strm->strpos_object)
	{
	  strm->strpos_object = new_str_pos(&strm->error_code);
	}
	 
	return (strm->strpos_object != NULL);
}

void load_rtf_data(const p_rtf_strm strm)
{
	return;
}

void close_rtf_strm(const p_rtf_strm rtf_strm)
{

	if (rtf_strm)
	{

	}

	return;

}

void get_document_overlay(const p_rtf_strm strm)
{
  puchar p;
  unsigned_int brace_count, size, size_left;


	p = strm->data + 1;

	brace_count = size = 1;

	size_left = strm->data_size - 1;

	while (true)
	{
		brace_count += (unsigned_int)(*p == '{') - (unsigned_int)(*p == '}'); // hahhahahahaha
		if ((!brace_count) || ((signed_int)--size_left <= 0)) break;
		size++;
		p++;
	}
		
	if ((signed_int)size_left > 1)
	{
	   if (p + 1 > strm->max_parsed_address)
	   {	
		 strm->overlay_data = p + 1;
		 strm->overlay_size = size_left - 1;
       }
	   else
	   {
	     strm->overlay_size = (unsigned_int)(strm->data + strm->data_size) - (unsigned_int)(strm->max_parsed_address + 1);
		 
		 if ((signed_int)strm->overlay_size <= 2)
		 {
		   strm->overlay_data = NULL;
		   strm->overlay_size = 0;
		 }
		 else strm->overlay_data = strm->max_parsed_address + 1;
		 
	   }
	}

	return;
}

static puchar locate_rtf_tag(const p_rtf_strm strm, const puchar buffer, const unsigned_int buffer_size, const t_rtf_tag tag, const boolean next)
{
  const t_rtf_control_name tag_names[MAX_RTF_TAG + 1] = {{NULL,0},{"\\object",7},{"\\objdata",8},{"\\fontemb",8},{"\\pict",5 },{"\\datafield",10},{"\\datastore",10},{"\\sv",3}};
  
  puchar p;
  t_string noodle, haystack;
  

	if ((tag == t_undefined_rtf_tag) || (tag > MAX_RTF_TAG))
	{
	  strm->error_code = API_ERROR;
	  return NULL;
	}

	noodle.type = haystack.type = T_ANSI_STRING_TYPE;
	noodle.utf_size = haystack.utf_size = 0;
	noodle.allocated_size = haystack.allocated_size = 0;

	noodle.len = tag_names[tag].name_len;
	noodle.str_direct_ptr = tag_names[tag].name;

	haystack.len = buffer_size;
	haystack.str_direct_ptr = buffer;

	if (next)
	{
		p = strm->strpos_object->next(strm->strpos_object);
	}
	else p = strm->strpos_object->first(strm->strpos_object, &noodle, &haystack, ADAPTATIVE_STRATEGY);

	strm->error_code = strm->strpos_object->error_code;

	return p;
}

boolean get_first_ole_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit;
  puchar p;


    strm->error_code = ERROR_SUCCESS;

	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_object_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 7;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_object_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_ole_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data += object->size;

	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 7) return false;

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_object_tag, false);

	if (!object->data) return false;

	object->data += 7;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_object_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_ole_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{
  unsigned_int size_left, braces_count;
  puchar p;

    
	strm->error_code = ERROR_SUCCESS;

	size_left = (unsigned_int)object->data + object->size;

	p = locate_rtf_tag(strm, object->data, object->size, t_rtf_object_data_tag, false);

	if (!p) return false;

	while (true)
	{
		object->data = p + 8;

		object->size = size_left - (unsigned_int)object->data;

		if ((signed_int)object->size <= 0) return false;

		p = locate_rtf_tag(strm, object->data, object->size, t_rtf_object_data_tag, true);

		if (!p) break;
	}

	rtf_memset(object_data, 0, sizeof(*object_data));

	object_data->data = object->data;

	size_left = object->size;

	p = object->data;

	braces_count = 1;

	while (true)
	{
		if (*(p - 1) != '\\') // /{ ou /} c'est des junks
		{
			switch (*p)
			{
			case '{': braces_count++;
				object_data->flags++;
				break;

			case '}': if (!braces_count) goto end;
				braces_count--;
				break;
			}
		}

		object_data->size++;

		size_left--;

		if (!size_left) break;

		p++;
	}

    end:

    return (object_data->size > 0);
}

boolean get_first_picture_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit;
  puchar p;


    strm->error_code = ERROR_SUCCESS;

	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_picture_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 5;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_picture_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_picture_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data += object->size;

	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 5) return false;

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_picture_tag, false);

	if (!object->data) return false;

	object->data += 5;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_picture_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_picture_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{
    
	rtf_memset(object_data,0,sizeof(*object_data));

	object_data->data = object->data;
	
	object_data->size = object_data->size_left = object->size;

    return true;
}

boolean get_first_font_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit;
  puchar p;

    strm->error_code = ERROR_SUCCESS;

	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_font_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 8;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_font_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_font_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data += object->size;

	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 8) return false; 

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_font_tag, false);

	if (!object->data) return false;

	object->data += 8;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_font_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_font_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{
	
	rtf_memset(object_data, 0, sizeof(*object_data));

	object_data->data = object->data;

	object_data->size = object_data->size_left = object->size;

	return true;
}

boolean get_first_datafield_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit;
  puchar p;


    strm->error_code = ERROR_SUCCESS;
	
	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_datafield_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 10;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_datafield_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_datafield_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


    strm->error_code = ERROR_SUCCESS;

	object->data += object->size;
	
	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 10) return false;

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_datafield_tag, false);

	if (!object->data) return false;

	object->data += 10;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_datafield_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_datafield_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{

	rtf_memset(object_data, 0, sizeof(*object_data));

	object_data->data = object->data;

	object_data->size = object_data->size_left = object->size;

	return true;
}

boolean get_first_data_storage_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit; 
  puchar p;


    strm->error_code = ERROR_SUCCESS;

	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_datastore_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 10;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_datastore_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_data_storage_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data += object->size;

	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 10) return false;

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_datastore_tag, false);

	if (!object->data) return false;

	object->data += 10;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_datastore_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_data_storage_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{
	
	rtf_memset(object_data, 0, sizeof(*object_data));

	object_data->data = object->data;

	object_data->size = object_data->size_left = object->size;

	return true;
}

boolean get_first_shape_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data = locate_rtf_tag(strm, strm->data + 4, strm->data_size - 4, t_rtf_shape_tag, false);

	if (!object->data) return false;

	limit = (unsigned_int)strm->data + strm->data_size;

	object->data += 3;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_shape_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_next_shape_object(const p_rtf_strm strm, const p_rtf_block object)
{
  unsigned_int limit = (unsigned_int)strm->data + strm->data_size;
  puchar p;


	strm->error_code = ERROR_SUCCESS;

	object->data += object->size;

	object->size = limit - (unsigned_int)object->data;

	if ((signed_int)object->size <= 3) return false;

	object->data = locate_rtf_tag(strm, object->data, object->size, t_rtf_shape_tag, false);

	if (!object->data) return false;

	object->data += 3;

	p = locate_rtf_tag(strm, strm->data, limit - (unsigned_int)object->data, t_rtf_shape_tag, true);

	if (!p)
	{
		object->size = limit - (unsigned_int)object->data;
	}
	else object->size = (unsigned_int)p - (unsigned_int)object->data;

	object->flags = 0;

	return true;
}

boolean get_shape_object_data(const p_rtf_strm strm, const p_rtf_block object, const p_rtf_object_data object_data)
{  
  unsigned_int read_bytes = 0;

    
	rtf_memset(object_data,0,sizeof(*object_data));

	object_data->data = execute_sv_control(strm,object->data,object->size,&read_bytes,object_data);

	if (!object_data->data) return false;

	object_data->size = object_data->size_left = object->size - read_bytes;

	return ((signed_int)object_data->size > 0);
	
}

static puchar skip_noise(puchar buffer, unsigned_int size_left, punsigned_int consumed_size)
{

	while (*buffer <= 0x20) // sachant que les autres chars sont 0x09,0x0a,0x0D
	{
		*consumed_size = *consumed_size + 1;
		buffer++;
		size_left--;
		if ((signed_int)size_left <= 0) return NULL;
	}

	return buffer;
}

static puchar skip_space_and_tab(puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes)
{

	while (true)
	{
		if ((*buffer != 0x20) && (*buffer != 0x09)) break;
		size_left--;
		buffer++;
		*consumed_bytes = *consumed_bytes + 1;
		if ((signed_int)size_left <= 0) return NULL;
	}

	return buffer;
}

static int rtf_atoi(puchar buffer, unsigned_int size_left, punsigned_int consumed_size)
{
  int sum, a;
  boolean neg_flag;


	neg_flag = false;

	if (*buffer == '-')
	{
		if (size_left == 1) return -1;

		neg_flag = true;

		*consumed_size = *consumed_size + 1;

		buffer++;

		size_left--;

		buffer = skip_space_and_tab(buffer, size_left, consumed_size);

		if (!buffer) return -1;

		size_left -= *consumed_size;

		if ((signed_int)size_left <= 0) return -1;

	}

	sum = 0;

	while (true)
	{
		a = (*buffer++ - 0x30);
		*consumed_size = *consumed_size + 1;
		if (a > 0x09) break;
		sum = sum * 10 + a;
		size_left--;
		if (!size_left) break;
	}

	if (neg_flag) return -sum;
	else return sum;
}

static boolean get_control_infos(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_cword_infos ctrl_infos)
{
  uchar c;


	ctrl_infos->ctrl_name = buffer;
	ctrl_infos->ctrl_name_len = 0;
	ctrl_infos->ctrl_param = NULL;
	ctrl_infos->ctrl_param_len = 0;

	while (true)
	{
		c = *buffer;

		if (!test_char(c, is_alpha)) break;

		ctrl_infos->ctrl_name_len++;

		*consumed_bytes = *consumed_bytes + 1;

		size_left--;

		if ((signed_int)size_left <= 0) return false;

		if (ctrl_infos->ctrl_name_len >= 250)
		{
			ctrl_infos->ctrl_name = NULL;
			goto fin;
		}

		buffer++;
	}

	if ((c == '-') || (test_char(c, is_digit)))
	{
		ctrl_infos->ctrl_param = buffer;
		ctrl_infos->ctrl_param_len++;
		*consumed_bytes = *consumed_bytes + 1;
						
		while ((signed_int)--size_left > 0)
		{
		  c = *(++buffer);

		  if (!test_char(c,is_digit)) break;

		  ctrl_infos->ctrl_param_len++;

		  *consumed_bytes = *consumed_bytes + 1;
		  		  
		}
	}

fin:

	return true;
}

static puchar discard_until_next_bracet(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes)
{

	while (true)
	{
		if ((*buffer == '{') || (*buffer == '}')) break;

		*consumed_bytes = *consumed_bytes + 1;

		size_left--;

		if ((signed_int)size_left <= 0)
		{
			buffer = NULL;
			break;
		}

		buffer++;
	}

	return buffer;
}

static boolean dump_data(const p_rtf_strm strm, const p_rtf_object_data object_data)
{
  unsigned_int len;
  puchar src;
  uchar odd_char;


	odd_char = 0;

	if (object_data->decoded_buffer.decoded_size & 1)
	{
		odd_char = object_data->decoded_buffer.buffer[--object_data->decoded_buffer.decoded_size];
	}

	if ((signed_int)object_data->decoded_buffer.decoded_size <= 0) return false;

	object_data->decoded_buffer.decoded_size /= 2;

	object_data->size_left -= object_data->decoded_buffer.decoded_size;

	if ((signed_int)object_data->size_left < 0)
	{
		strm->error_code = BUFFER_TOO_SMALL_ERROR;
		return false;
	}

	src = (puchar)&object_data->decoded_buffer.buffer;

	len = object_data->decoded_buffer.decoded_size;

	object_data->decoded_size += len;

	while (len >= 4)
	{
	  *(puint32)object_data->decoded_data = hex_to_dword(src);
	  
	  object_data->decoded_data += 4;
		
	  len -= 4;
		
	  if (!len) break;
		
	  src += 8;
	}

	while (len)
	{
		*(puint8)object_data->decoded_data++ = hex_to_byte(src);
		if (--len == 0) break;
		src += 2;
	}

	if (odd_char)
	{
		object_data->decoded_buffer.decoded_size = 1;
		object_data->decoded_buffer.buffer[0] = odd_char;
	}
	else object_data->decoded_buffer.decoded_size = 0;

	return true;
}

static puchar read_bin_control(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data)
{
  unsigned_int index, rem, num,safe_size;
  uchar bin_buffer[MAXBYTE];


	rem = 0;

	buffer = skip_space_and_tab(buffer - 1, size_left + 1, &rem);

	if (!buffer) return NULL;

	*consumed_bytes = *consumed_bytes + rem;

	size_left -= rem;

	if ((signed_int)size_left <= 0) return NULL;

	index = 0;

	rem = sizeof(bin_buffer) - 5;

	while (true)
	{
		switch (*buffer)
		{
		case 0x09:
		case 0x0A:
		case 0x0D:
		case 0x20:
		case '\\': goto fin;
		}

		bin_buffer[index++] = *buffer++;

		size_left--;

		*consumed_bytes = *consumed_bytes + 1;

		if ((signed_int)size_left <= 0) return NULL;

		rem--;

		if (!rem)
		{
			return buffer;
		}
	}

fin:

	safe_size = rem;

	rem = 0;

	num = rtf_atoi(&bin_buffer, index, &rem);

	if ((signed_int)num < 0)
	{	  		
	    strm->malformed_document++;
				
		buffer = skip_noise(buffer - safe_size, size_left + safe_size, &rem);
		
		if (buffer)
		{
			*consumed_bytes = (*consumed_bytes + rem) - safe_size;
		}

		return buffer;
	}

	if (num)
	{
		if (num > object_data->size_left)
		{
			strm->error_code = BUFFER_TOO_SMALL_ERROR;
			return NULL;
		}

		*consumed_bytes = *consumed_bytes + num;

		size_left -= num;

		if ((signed_int)size_left < 0) return NULL;

		rem = 0;

		buffer = skip_noise(buffer, size_left, &rem);

		if (!buffer) return NULL;

		*consumed_bytes = *consumed_bytes + rem;

		size_left -= rem;

		if ((signed_int)size_left < 0) return NULL;

		if (object_data->decoded_buffer.decoded_size >= sizeof(object_data->decoded_buffer.buffer)) // flush the cache first
		{
			if (!dump_data(strm, object_data)) return NULL;
			object_data->decoded_buffer.decoded_size = 0;
		}

		object_data->decoded_size += num;

		rtf_memcpy(object_data->decoded_data, buffer, num);

		buffer += num;
	}

	return buffer;

}

static puchar execute_sv_control(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data)
{
  unsigned_int semicolon_count = 0;
  
  	
	while (true)
	{
	  switch (*buffer)
	  {
	    case '{' : 
		case '}' : return buffer;

		case ';' : semicolon_count += (unsigned_int)((boolean)(*buffer++ == ';'));
		           break;
	  }
	  buffer++;
	  *consumed_bytes = *consumed_bytes + 1;
	  size_left--;
	  if ((semicolon_count >= 2) ||	((signed_int)size_left <= 0)) break;
	}
	
	if ((signed_int)size_left <= 0) return NULL;

	return buffer;
}

static puchar consume_data(const p_rtf_strm strm, puchar buffer, unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data)
{

	while (true)
	{
		if ((*buffer == '}') || (*buffer == '\\')) break;

		*consumed_bytes = *consumed_bytes + 1;

		size_left--;

		if ((signed_int)size_left <= 0)
		{
			buffer = NULL;
			break;
		}

		buffer++;
	}

	return buffer;
}

static puchar execute_rtf_control(const p_rtf_strm strm, const puchar buffer, const unsigned_int size_left, const punsigned_int consumed_bytes, const p_rtf_object_data object_data)
{
  p_tag_def_table_entry control_entry;
  p_tag_definition match_entry;
  unsigned_int i, asterisk_flag;
  t_rtf_cword_infos ctrl_infos;
  uchar c;


	asterisk_flag = 0;

	c = *buffer;

	switch (c)
	{
	case 0x27: i = MACRO_MIN_VALUE(size_left, 3);

		if (TEST_ATTRIBUTE(object_data->decoded_buffer.decoded_size, 3))
		{
			object_data->decoded_buffer.decoded_size--;
		}

		*consumed_bytes = *consumed_bytes + i;

		return buffer + i;

	case '*': c = *(buffer + 1);

		asterisk_flag++;

		strm->last_state = strm->current_state;
		strm->current_state = t_rtf_asterisk_state;

	default: if ((c >= 'a') && (c <= 'z'))
	{
		unsigned_int read_bytes = asterisk_flag;

		if (!get_control_infos(strm, buffer + asterisk_flag, size_left - asterisk_flag, &read_bytes, &ctrl_infos)) return NULL;

		*consumed_bytes = *consumed_bytes + read_bytes;

		if (!ctrl_infos.ctrl_name)
		{
			return buffer + read_bytes;
		}

	}
			 else
			 {
				 *consumed_bytes = *consumed_bytes + 1;

				 return buffer + 1;
			 }

	}

	c -= 'a';

	control_entry = tag_defs_lookup[c];

	if ((!control_entry) || ((signed_int)control_entry->tags_count <= 0) || (!control_entry->tag_defs))
	{
		return buffer + (ctrl_infos.ctrl_name_len + ctrl_infos.ctrl_param_len);
	}

	match_entry = NULL;

	i = control_entry->tags_count - 1;

	while (true)
	{
		if ((control_entry->tag_defs[i].len == ctrl_infos.ctrl_name_len) && (rtf_memcmp(control_entry->tag_defs[i].name, ctrl_infos.ctrl_name, ctrl_infos.ctrl_name_len)))
		{
			match_entry = &control_entry->tag_defs[i];
			break;
		}

		i--;

		if ((signed_int)i < 0) break;
	}

	i = ctrl_infos.ctrl_name_len + ctrl_infos.ctrl_param_len + asterisk_flag;

	if (!match_entry)
	{
		if (strm->last_state != t_rtf_asterisk_state)
		{
			return buffer + i;
		}
		else return discard_until_next_bracet(strm, buffer + i, size_left - i, consumed_bytes);
	}
	else return match_entry->callback(strm, buffer + i, size_left - i, consumed_bytes, object_data);
}

boolean read_object_data(const p_rtf_strm strm, const p_rtf_object_data object_data)
{
  puchar src, dest;
  unsigned_int size_left, braces_count;


	strm->error_code = ERROR_SUCCESS;

	strm->current_state = strm->last_state = t_rtf_undefined_state;

	src = object_data->data;

	dest = object_data->decoded_data;

	size_left = object_data->size;

	object_data->decoded_buffer.decoded_size = 0;

	braces_count = 0;

	while (true)
	{
	debut:

		switch (*src)
		{
		case '{': braces_count++;

			strm->last_state = strm->current_state;
			strm->current_state = t_rtf_open_brace_state;

			break;

		case '}': strm->last_state = strm->current_state;
			strm->current_state = t_rtf_close_brace_state;

			if ((signed_int)braces_count <= 0) goto fin;

			braces_count--;

			break;

		case '\\': {
			unsigned_int consumed_bytes = 0;

			strm->last_state = strm->current_state;
			strm->current_state = t_rtf_execute_control_state;

			src = execute_rtf_control(strm, src + 1, size_left - 1, &consumed_bytes, object_data);

			if ((!src) || (!consumed_bytes))
			{
				return false;
			}

			size_left -= consumed_bytes;

			if ((signed_int)size_left <= 0) goto fin;

			goto debut;
		}

				   break;

		default: {
			unsigned_int consumed_size = 0;
			uchar c;

			strm->last_state = strm->current_state;
			strm->current_state = t_rtf_handle_char_state;

			src = skip_noise(src, size_left, &consumed_size);

			if (!src) goto fin;

			consumed_size = MACRO_MAX_VALUE(1, consumed_size);

			size_left -= consumed_size;

			if ((signed_int)size_left <= 0) goto fin;

			c = *src;

			if (!test_char(c, is_alnum)) goto debut;

			if (object_data->decoded_buffer.decoded_size >= sizeof(object_data->decoded_buffer.buffer))
			{
				if (!dump_data(strm, object_data)) return false;
			}

			object_data->decoded_buffer.buffer[object_data->decoded_buffer.decoded_size++] = c;

			break;
		}
		}

		src++;
	}

fin:

	if (strm->error_code != ERROR_SUCCESS) return false;

	strm->max_parsed_address = MACRO_MAX_VALUE(strm->max_parsed_address,src);

	if (object_data->decoded_buffer.decoded_size)
	{
	  return dump_data(strm, object_data);
	}
		
	return (object_data->decoded_size != 0);
}

