Name="Mine";
Description="����ѡ��������һ�����ף��ڵо�ռ�������󴥷����۳��Է�1~3����������";
ImageName="Card/Mine";
function Use()
	choose1=GameStateManager:GetChoose(1);
	choose2=GameStateManager:GetChoose(2);
	if(choose1~=-1 and choose2==-1) then
		area=Terrain:GetArea(choose1);
		if(area:GetAreaBelong()==GameStateManager:GetCurrentPlayer():GetID()) then
			buff=BuffManager:CreateNewBuff("Mine");
			area:AddBuff(buff);
			re=1;
			print(Name.."-Use");
		else
			print(Name.."-�������ڸ����");
		end
	else
		print(Name.."-û��ѡ������");
		re=0;
	end
	return re;
end