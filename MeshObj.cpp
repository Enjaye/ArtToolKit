#include "MeshObj.h"


MeshObj::MeshObj(string s,MeshObj *first)
{
    charger_obj(s,first);
}
MeshObj::~MeshObj()
{
    free(vertice);
    free(normals);
    free(textures);
    free(colours);

    for(unsigned int i=0;i<materiaux.size();i++)
        delete materiaux[i];
    materiaux.clear();
}


string doubleSlash(string s)
{
    //Remplace "//" par "/1/".
    string s1="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(i<s.size()-1&&s[i]=='/'&&s[i+1]=='/')
        {
            s1+="/1/";
            i++;
        }
        else
            s1+=s[i];
    }
    return s1;
}

string remplacerSlash(string s)
{
    //Remplace les '/' par des espaces.
    string ret="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(s[i]=='/')
            ret+=' ';
        else
            ret+=s[i];
    }
    return ret;
}


vector<string> splitSpace(string s)
{
    //Eclate une chaîne au niveau de ses espaces.
    vector<string> ret;
    string s1="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(s[i]==' '||i==s.size()-1)
        {
            if(i==s.size()-1)
                s1+=s[i];
            ret.push_back(s1);
            s1="";
        }
        else
            s1+=s[i];
    }
    return ret;
}


string get_directory(string s)
{
    string s1="",s2="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(s[i]=='/'||s[i]=='\\')
        {
            s1+=s2+"/";
            s2="";
        }
        else
            s2+=s[i];
    }
    return s1;
}

float* vector2float(vector<float>& tableau)
{
    float* t=NULL;
    t=(float*)malloc(tableau.size()*sizeof(float));
    if(t==NULL||tableau.empty())
    {
        float *t1=(float*)malloc(sizeof(float)*3);
        for(int i=0;i<3;i++)
            t1[i]=0.;
        return t1;
    }

    for(unsigned int i=0;i<tableau.size();i++)
        t[i]=tableau[i];
    return t;
}

//on a rajouté un boolean : couleur ou non? si non on est en mode picking, donc on n'affiche pas les couleurs ni les textures de l'objet.
void MeshObj::draw_model(bool nor,bool tex, bool color)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if(nor && color)glEnableClientState(GL_NORMAL_ARRAY);
    if(tex && color)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D,texture);
    }

    if(color)glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,vertice);

    if(tex && color)glTexCoordPointer(2,GL_FLOAT,0,textures);
    if(nor && color)glNormalPointer(GL_FLOAT,0,normals);
    if(color)glColorPointer(4,GL_FLOAT,0,colours);

    glDrawArrays(GL_TRIANGLES,0,n_data);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}



void MeshObj::charger_mtl(std::string nom)
{
    ifstream fichier(nom.c_str(),ios::in);
    string curname="";
    string ligne="";

    if(fichier)
    {
        while(getline(fichier, ligne))
        {
            if(ligne[0]=='n') //nouveau materiau
                curname=ligne.substr(7);
            else if(ligne[0]=='K'&&ligne[1]=='d') //couleur
            {
                vector<string> termes=splitSpace(ligne.substr(3));
                materiaux.push_back(new Material((float)strtod(termes[0].c_str(),NULL),(float)strtod(termes[1].c_str(),NULL),(float)strtod(termes[2].c_str(),NULL),curname));
            }
            else if(ligne[0]=='m'&&ligne[5]=='d')//map_Kd (texture)
            {
                string f=get_directory(nom)+ligne.substr(7);
    //            texture=loadTexture(f.c_str());
            }
            else if(ligne[0]=='d') //opacité
            {
                string n=ligne.substr(2);
                materiaux[materiaux.size()-1]->coul.a=strtod(n.c_str(),NULL);
            }
        }
        fichier.close();
    }
    else
    {
        cout<<"ERREUR: echec ouverture fichier mtl "<<endl;
    }

}

void MeshObj::charger_obj(std::string nom,MeshObj *first)
{
    vector<FloatVector> ver,nor,tex,col;
    vector<unsigned int> iv,it,in;
    string ligne,curname="";

    ifstream fichier(nom.c_str(),ios::in);

    if(fichier)
    {
        string ligne;
		
        while(getline(fichier, ligne))
        {
            if(ligne[0]=='v') //Coordonnées de points (vertex, texture et normale)
            {
                if(ligne[1]==' ') //Vertex
                {
                    char x[255],y[255],z[255];
                    sscanf(ligne.c_str(),"v %s %s %s",x,y,z);
                    ver.push_back(FloatVector(strtod(x,NULL),strtod(y,NULL),strtod(z,NULL)));

                }
                else if(ligne[1]=='t') //Texture
                {
                    char x[255],y[255];
                    sscanf(ligne.c_str(),"vt %s %s",x,y);
                    tex.push_back(FloatVector(strtod(x,NULL),strtod(y,NULL)));
                }
                else if(ligne[1]=='n') //Normale
                {
                    char x[255],y[255],z[255];
                    sscanf(ligne.c_str(),"vn %s %s %s",x,y,z);
                    nor.push_back(FloatVector(strtod(x,NULL),strtod(y,NULL),strtod(z,NULL)));
                }
            }
            else if(ligne[0]=='f') //Indice faces
            {
                ligne=doubleSlash(ligne); //On remplace "//" par "/1/" dans toute la ligne
                ligne=remplacerSlash(ligne); //On remplace les '/' par des espaces, ex : pour "f 1/2/3 4/5/6 7/8/9" on obtiendra "f 1 2 3 4 5 6 7 8 9"

                vector<string> termes=splitSpace(ligne.substr(2)); //On éclate la chaîne en ses espaces (le substr permet d'enlever "f ")

                //int ndonnees=(int)termes.size()/3;
                for(int i=0;i<3;i++)
                {
                    iv.push_back(strtol(termes[i*3].c_str(),NULL,10)-1);
                    it.push_back(strtol(termes[i*3+1].c_str(),NULL,10)-1);
                    in.push_back(strtol(termes[i*3+2].c_str(),NULL,10)-1);
                }


                for(unsigned int i=0;i<materiaux.size();i++)
                {
                    if(materiaux[i]->name==curname)
                    {
                        for(int j=0;j<3;j++)
                            col.push_back(materiaux[i]->coul); //On ajoute la couleur correspondante
                        break;
                    }
                }
            }
            else if(ligne[0]=='m'&&first==NULL)//fichier MTL et si c'est la première frame (comme ça on ne charge pas plusieurs fois le même MTL et la même texture)
            {
                 charger_mtl(get_directory(nom)+ligne.substr(7));
            }
            else if(ligne[0]=='u')//utiliser un MTL
            {
                curname=ligne.substr(7);
            }
        }

            fichier.close();

            vector<float> tv(0),tc(0),tn(0),tt(0);
            for(unsigned int i=0;i<iv.size();i++)
                if(iv[i]<ver.size())
                {
                    tv.push_back(ver[iv[i]].x);
                    tv.push_back(ver[iv[i]].y);
                    tv.push_back(ver[iv[i]].z);
                    if(col.size()>0)
                    {
                        tc.push_back(col[i].x);
                        tc.push_back(col[i].y);
                        tc.push_back(col[i].z);
                        tc.push_back(col[i].a);
                    }
                }

            for(unsigned int i=0;i<in.size();i++)
                if(in[i]<nor.size())
                {
                    tn.push_back(nor[in[i]].x);
                    tn.push_back(nor[in[i]].y);
                    tn.push_back(nor[in[i]].z);
                }

            for(unsigned int i=0;i<it.size();i++)
                if(it[i]<tex.size())
                {
                    tt.push_back(tex[it[i]].x);
                    tt.push_back(tex[it[i]].y);
                }

        vertice=vector2float(tv);
        normals=vector2float(tn);
        textures=vector2float(tt);
        colours=vector2float(tc);

        n_data=iv.size();

        ver.clear();
        nor.clear();
        tex.clear();
        col.clear();

        iv.clear();
        it.clear();
        in.clear();

		cout <<"L'objet a ete charge avec succes"<<endl;
    }
    else
    {
        cout<<"ERREUR: ouverture fichier obj impossible"<<endl;
    }
}

