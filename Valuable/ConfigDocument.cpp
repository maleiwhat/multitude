#include "ConfigDocument.hpp"
namespace Valuable
{  
  ConfigDocument::ConfigDocument(void)
  {
  }

  ConfigDocument::~ConfigDocument(void)
  {
  }
  void ConfigDocument::readConfigFile(char *fileName)
  {
    string str;

    ifstream input (fileName);

    int depth=0;
    bool flag=false,atFlag=false;

    int k=0;
    while(std::getline(input,str)){
      if(str!="")
	if(parseLine(str)==ELEMENT_START)
	  {

	    string s;
	    depth++;
	    Element elm,e;
	    size_t ps=str.find(",");
	    if(ps<10000)
	      {

		e.elementName=str.substr(0,ps);
		size_t ps2=str.find("{");
		e.type=str.substr(ps+1,ps2-ps-1);

	      }
	    else
	      {

		e.elementName=str.substr(0,str.find("{")-1);
	      }
	    //e.elementName=str.substr(0,str.length()-2);
	    e.depth=depth;
	    elm.Nodes.push_back(e);
	    k++;



	    while(std::getline(input,s))
	      {
		if(s!="")
		  if(parseLine(s)==ELEMENT_START)
		    {
		      depth++;
		      Element child=Element();
		      size_t ps=s.find(",");
		      if(ps<10000)
			{

			  child.elementName=s.substr(0,ps);
			  child.type=s.substr(ps+1,s.find("{")-ps-1);

			}
		      else
			{

			  child.elementName=s.substr(0,s.find("{")-1);
			}
		      child.depth=depth;
		      elm.Nodes.push_back(child);
		      k++;

		    }
		  else if(parseLine(s)==ATTRIBUTE)
		    {
		      Attribute att=Attribute();
		      for(int i = 0; i < (int) s.length();i++)
			if(s[i]=='\"')
			  s[i]=' ';
		      size_t pos=s.find("=");

		      att.key=s.substr(0,pos);
		      att.value=s.substr(pos+1,s.length());

		      att.depth=depth;
		      elm.Nodes[k-1].Attributes.push_back(att);


		    }
		  else if(parseLine(s)==ELEMENT_END)
		    {depth--;


		      if(depth==0)
			{
			  k=0;

			  for(int i=elm.Nodes.size()-1;i>0;i--)
			    {
			      if(elm.Nodes[i].depth>elm.Nodes[i-1].depth)
				{
				  elm.Nodes[i-1].Nodes.push_back(elm.Nodes[i]);

				}
			      else
				{

				  for(int j=i-2;j>=0;j--)
				    {
				      if(elm.Nodes[j].depth<elm.Nodes[i].depth && (elm.Nodes[i].depth-elm.Nodes[j].depth)==1 )
					{
					  elm.Nodes[j].Nodes.push_back(elm.Nodes[i]);
					  break;
					}


				    }
				}


			    }


			  Element el= Element();
			  el.Nodes.push_back(elm.Nodes[0]);
			  Elements.Nodes.push_back(el);
			  elm.Nodes.clear();
			  elm.Attributes.clear();
			  flag=false;
			  atFlag=false;

			}
		    }
	      }

	  }
    }
    input.close();
  }
  void ConfigDocument::writeConfigFile(char *fileName)
  {
    ofstream output(fileName);
    vector<string> ss;
    string aa=getConfigText(Elements,ss);
    //	for(int i=ss.size()-1;i>=0;i--)
    //		output<<ss[i];
    output<<aa;
    output.close();


  }
  void ConfigDocument::TrimSpaces( std::string& str)  
  {  
    using namespace std;
    size_t startpos = str.find_first_not_of(" \t"); 
    size_t endpos = str.find_last_not_of(" \t"); 

    if(( string::npos == startpos ) || ( string::npos == endpos))  
      {  
	str = "";  
      }  
    else  
      str = str.substr( startpos, endpos-startpos+1 );  
 
  }
  Element *ConfigDocument::getElement(string elementName)
  {
    bool found=false;
		
    Element *f=findElement(Elements,elementName,found);
    if(f)
      return f;
    else
      return 0;

  }
  Element *ConfigDocument::getElement(string key,string value)
  {
    bool found=false;
		
    Element *f=findElement(Elements,found,key,value);
    if(f)
      return f;
    else
      return 0;

  }
  Element *ConfigDocument::findElement(Element &e,bool &found,string key,string value)
  {
    for(int i=0;i < (int)e.Nodes.size() ;i++)
      {
	Element *w;
	w=findElement(e.Nodes[i],found,key,value);
	if(found)
	  return w;

      }

    for(int j=0;j<(int)e.Attributes.size();j++)
      {
	string ke=e.Attributes[j].key;
	TrimSpaces(ke);
	string val=e.Attributes[j].value;
	TrimSpaces(val);
	if(key==ke && value==val)
	  {
	    found=true;

	    return &e;

	  }
      }

    return 0;
  }

  Element *ConfigDocument::findElement(Element &e,string elementName,bool &found)
  {
    for(int i=0;i < (int)e.Nodes.size() ;i++)
      {
	Element *w;
	w=findElement(e.Nodes[i],elementName,found);
	if(found)
	  return w;

      }

    string s=e.elementName;
    TrimSpaces(s);
    if(s==elementName)
      {
	found=true;
		
	return &e;
			 
      }

    return 0;
  }
  string ConfigDocument::getConfigText(Element e,vector<string> &s)
  {
    string str;

    //for(int i=e.Nodes.size()-1;i>=0;i--)

    //	if(e.elementName!="")
    {
      if(e.elementName!="")
	{
	  if(e.type=="")
	    str+=e.elementName+" {";
	  else
	    str+=e.elementName+","+e.type+" {";
	  str+="\n";
	}

      for(int j=0;j < (int) e.Attributes.size();j++)
	{
	  TrimSpaces(e.Attributes[j].value);
	  str+=e.Attributes[j].key+"="+"\""+e.Attributes[j].value+"\""+"\n";

	}
      for(int i = 0; i < (int) e.Nodes.size(); i++)
	str+=getConfigText(e.Nodes[i],s);

      if(e.elementName!="")
	{
	  str+="}";
	  str+="\n";
	}
      //s.push_back(str);
    }
    return str;

  }
  ConfigDocument::ParseFlags ConfigDocument::parseLine(string line)
  {

    if(line[line.length()-1]=='{')
      return ELEMENT_START;
    else if(line[line.length()-1]=='}')
      return ELEMENT_END;
    else
      {
	size_t pos=line.find("=");
	if(pos>=10000)
	  return NOT_VALID;
	else
	  return ATTRIBUTE;
      }
  }

}
