#version 330
out  vec4 vFragColor;
in vec4 Color;//插值后每个片元的颜色
uniform sampler3D tex;//体数据纹理对象
//uniform sampler3D gtex;
uniform sampler2D backtex;//帧纹理
uniform sampler1D tf;//tranform 纹理对象
// texture of color map
uniform sampler2D colortex;//颜色纹理对象
//texture of random 
uniform sampler2D random;//随机纹理对象
//texture of directIllumination
uniform sampler3D gIllTex;//光照纹理对象
//float result_en = 0.0f;

uniform int width;
uniform int height;
uniform int step;
uniform int gradientStep;
uniform vec3 eyePositionVec3;
uniform int light;
uniform vec3 ambient;//光源0环境光系数(带颜色)
uniform vec3 diffuse;//光源0的漫反射光系数
uniform vec3 specular;//光源0的镜面反射光系数
uniform vec3 backgroundColor;//背景颜色
uniform vec3 lightPositionAfterVec3;
uniform float surfaceShadingParameter;
//float surfaceShadingParameter = 1.0f;
//HDR is on ro off
uniform bool hdrOn;
//if use tf colors
uniform bool useTFColors;
//HDR exposure parameter
uniform float exposure;
//gamma correction
uniform float gammaCorrection;
//light attenuation
uniform float KL;
float KQ = 0.5;
float voxelToLightDis = 0.0;
float lightAttenuation = 1.0;
//the direction from voxel to light
vec3 voxelToLightDir;
//color box
uniform float colorBox;
//data to exact
uniform float dataToExact;
//gradient
vec3 gradientVec3;
/************************** func****************************/
//func that get gradient
vec3 getGradientVec3(vec3 pos);
/************************** func****************************/
//对于3D纹理，利用每一个体素的颜色来作为其坐标
vec4 backColor = texture(backtex,vec2(gl_FragCoord.x/width,gl_FragCoord.y/height));//背景帧缓存颜色，刚开始是全黑的，当改变窗口大小，或者改变数据时，背景缓存的颜色有变
vec3 frontToBackVec3 = vec3(backColor.x-Color.x,backColor.y-Color.y,backColor.z-Color.z);//用纹理颜色与背景颜色的差值来作为向量(OB-OF=FB)

//vec3 frontToBackVec3 = vec3(Color.x - eyePos.x,Color.y - eyePos.y,Color.z - eyePos.z);
vec3 begin = vec3(Color);//片源的rgb值(每一个体素块的rgb值)
float frontToBackLengthF = length(frontToBackVec3);////
float rayCastStepF = 1.73f/float(step);//step采样点的数量，raycasting下每个射线的采样个数，这个值是每两个采样点之间的距离
float gradientStepLen = 1.73f/float(gradientStep);//梯度采样
vec3 rayCastDirVec3 = normalize(frontToBackVec3)*rayCastStepF;//在这个方向上每两个采样点之间的向量值(FB颜色差)


vec3 getPhongVec3()//Phong方法下计算出来的光照颜色。
{
    vec3 color;
	//float _surfaceShadingParameter = length(normalize(getGradientVec3(begin)));
	vec3 N = normalize(gradientVec3);
	vec3 L = normalize(voxelToLightDir);
	vec3 V = normalize(eyePositionVec3);
	vec3 H = normalize(V + L);
	float NH = dot(N,H);
	float NL = dot(L,N);
	if(NL < 0.0f)
		NL = -NL;
	//if(NL < 0.5)
	//	surfaceShadingParameter = 0.0;
	if(NH < 0.0f)
		NH = 0.0f;
    color = vec3(1.0f - surfaceShadingParameter) + (NL*diffuse + pow(NH,16)*specular)*surfaceShadingParameter;
	
    return color; 
}
//fuc that get color
vec4 getColor()
{
   float alphaAccumlation = 1.0f;//透明度累加器，第一次是1，之后会乘上(1-a)，a为上一次灰度值对应的TF中的透明度。
   vec3  colorAccumlation = vec3(0.0f);//颜色累加器
   
   float randomStep = texture(random,vec2(gl_FragCoord.x/256.0f,gl_FragCoord.y/256.0f)).a;//随机纹理与背景同样位置对应的颜色中的a值。
   begin += rayCastDirVec3*randomStep;//片源的初始颜色值+每两个采样点之间的颜色差*一个系数
   for(float i = 0.0f;i <= frontToBackLengthF;i+=rayCastStepF,begin+=rayCastDirVec3)//遍历这个ratcasting上的所有采样点，每循环一次，begin就会增加一个两个采样点之间的颜色差来作为三维位置。
	{
                float alpha = texture(tex,begin).a * dataToExact;//当前begin对应的要渲染的3D纹理中的a值(也是灰度值)
		
                vec4 Vol =  texture(tf,alpha);//这个a值对应的TF数据中的透明度。
		if(!useTFColors)	 
                        Vol.rgb = texture(colortex,vec2(0.55 + colorBox,0.3f)).bgr;//对于Vol中的前三个RGB用要使用的颜色画板颜色进行覆盖，(材质的颜色)
                alpha = Vol.a;//更新alpha,TF中的透明度
                voxelToLightDir = lightPositionAfterVec3 - (begin * 2.0f - 1.0f);//由体素指向灯光的向量
                voxelToLightDis = length(voxelToLightDir);//长度
                lightAttenuation = 1.0/(1.0  + KQ * voxelToLightDis * KL + KL * voxelToLightDis* voxelToLightDis);//衰减，
                gradientVec3 = getGradientVec3(begin);	//用梯度来代替表面法向量
                if( 0.0 == alpha)//不透明
		{
			continue;
		}		
                if( 7 == light)//indirectPhongIllum
		{ 
                       colorAccumlation += alphaAccumlation*Vol.rgb/*材质的颜色*/*alpha*(getPhongVec3())*( ambient/*光源的环境光颜色系数*/ +  texture(gIllTex,begin).rgb);

		}
                else if( 1 == light)//localLight
		{
                       colorAccumlation += alphaAccumlation*Vol.rgb*alpha*getPhongVec3() ;
			
		}
                else if(2 == light)//directLight
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha*( ambient + texture(gIllTex,begin).a);
		}
                else if(3 == light)//indrectLight
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha*texture(gIllTex,begin).rgb;
		}
                else if(4 == light)//globalLight
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha*texture(gIllTex,begin).rgb*(ambient + texture(gIllTex,begin).a) ;
		}
                else if(5 == light )//directPhongIllum
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha*getPhongVec3()*( ambient + texture(gIllTex,begin).a);
		}
                else if(6 == light)//globalPhongIllum
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha*getPhongVec3()*texture(gIllTex,begin).rgb*( ambient + texture(gIllTex,begin).a);
		}
                else if(0 == light)//noLight
		{
			colorAccumlation += alphaAccumlation*Vol.rgb*alpha;
		}
		alphaAccumlation *= (1.0f - alpha);
		
		if( alphaAccumlation < 0.01)
			break;
   	   
	}	
    vec4 result;
	vec3  r;	
	result = vec4(colorAccumlation * lightAttenuation,1.0f);
	if(hdrOn)
	{
		r = vec3(1.0) - exp(-vec3(result) * exposure);
	}else
	{
		r = vec3(result); 
	}
	r = pow(r,vec3(1.0/gammaCorrection));	
	//accunlate with backgroundColor
	r += backgroundColor.rgb * alphaAccumlation ;
	return vec4(r,1.0);
}
void main(void)
{
   
    vFragColor = getColor();
	
   
}
/////////////////////////////////////////////////////
//get the gradient////////////////////////////////
vec3 getGradientVec3(vec3 pos)
{
	//central difference gradient
	/*
    float fVolumeValXp = texture(tex,pos + vec3(+rayCastStepF,0,0)).a * dataToExact;
    float fVolumeValXm = texture(tex,pos + vec3(-rayCastStepF,0,0)).a * dataToExact;
    float fVolumeValYp = texture(tex,pos + vec3(0,+rayCastStepF,0)).a * dataToExact;
    float fVolumeValYm = texture(tex,pos + vec3(0,-rayCastStepF,0)).a * dataToExact;
    float fVolumeValZp = texture(tex,pos + vec3(0,0,+rayCastStepF)).a * dataToExact;
    float fVolumeValZm = texture(tex,pos + vec3(0,0,-rayCastStepF)).a * dataToExact;
	
    return vec3(fVolumeValXp - fVolumeValXm,
              fVolumeValYp - fVolumeValYm,
              fVolumeValZp - fVolumeValZm ) / 2.0;
	*/	  
	//forward difference gradient
	float fVolumeVal =   texture(tex,pos + vec3(0,0,0)).a * dataToExact;
	float fVolumeValXm = texture(tex,pos + vec3(-rayCastStepF,0,0)).a * dataToExact;
	float fVolumeValYm = texture(tex,pos + vec3(0,-rayCastStepF,0)).a * dataToExact;
	float fVolumeValZm = texture(tex,pos + vec3(0,0,-rayCastStepF)).a * dataToExact;
	
	 return vec3(fVolumeVal - fVolumeValXm,
              fVolumeVal - fVolumeValYm,
              fVolumeVal - fVolumeValZm );
}

//////////////////////////////////////////////////
