from datetime import datetime
from queue import LifoQueue, Queue

from django.db.models import Q
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.contrib.auth.models import User
from .models import Material, Input, Output, Remain, Compose
from . import models

# Create your views here.
def index(request):
    return HttpResponse("Hello, world. You're at the polls index.你正处于选择功能页面："
                        "有以下两个功能"
                        "1：MRP计算（输入一条记录-产品名称+数量+完工日期）（输出调配方式+物料号+物料名称+需求数量+日程下达日期+日程完成日期）"
                        "2：公式推导（输入一个变量b1）（输出b1=a7+a9）"
                        "优秀92分以上：多条记录")
def dashboard(request):
    user_count = User.objects.count()
    material_count = Material.objects.count()
    input_count = Input.objects.count()
    output_count = Output.objects.count()
    context = {'user_count': user_count, 'material_count': material_count,'input_count':input_count,'output_count':output_count}
    return render(request, 'app01/dashboard.html', context)
def input(request):
    return render(request, "app01/input.html")

def CreateRoot(MatNeed):
    name=MatNeed.name
    pro=(Material.objects.filter(MatName=name).first()).MatPro
    number=MatNeed.number
    comdate=MatNeed.date
    zday=0
    tday=0
    if pro == 'e':
        zday=(Material.objects.filter(MatName=name).first()).MatPre
    else:
        tday=(Allo.objects.filter(ChiName=name).first()).MatPre+(Allo.objects.filter(ChiName=name).first()).ShoP
    pday=zday+tday
    b = datetime.datetime.strptime(MatNeed.date, '%Y-%m-%d')
    predate = datetime.datetime.strftime((b - datetime.timedelta(days=pday)), '%Y-%m-%d')
    root=TreeNode(name, pro, number, predate, comdate)
    return root

class TreeNode:
    def __init__(self, name, production_method, quantity, predate, comday):
        self.name = name
        self.production_method = production_method
        self.quantity = quantity
        self.predate = predate
        self.comday = comday
        self.chilist = []

def DFS(Nod) :
    Nod.chilist.clear()
    ChiM=Compose.objects.filter(父物料名称=Nod.name)
    if len(ChiM) != 0:
        for i in range_(0,len(ChiM)):
            name=ChiM[i].ChiName
            malos=((Material.objects.filter(名称=name)).first()).MatLos
            num = math.ceil((ChiM[i].ConNum*Nod.number/(1-malos)))
            #num = ChiM[il ConNum * Nod.number
            comday=Nod. predate
            pro = (Material.objects.filter(名称=name).first()).MatPro
            zday = 0
            tday = 0
            if pro == '#*':
                zday = (Material.objects. filter(MatName=name).first()).MatPre
            else:
                tday = (Allo.objects. filter(ChiName=name).first()).MatPre + (
                    Allo. objects.filter(ChiName=name).first()). ShoPre
            pday = zday + tday
            b = datetime.datetime.strptime(comday, '%Y-m-%d')
            predate = datetime.datetime.strftime((b - datetime.timedelta(days=pday)), '%Y-%m-%d')
            Node=TreeNode(name,pro,num,predate,comday)
            Nod.chilist.append(Node)
            DFS(Node)  #深度优先
    return None
#开工日期早先拿到库存
def EnsureTree (name):
    q = LifoQueue()  # 后进先出队列
    index_of_min=0
    date=datetime.date.today()
    mindate=datetime.date.today()
    for i in range(0,len(Tree_list)):
        q.put(Tree_list[i])
        while q.empty() != True:
            node=q.get()
            if node.name == name:
                date=node.predate
                while not q.empty():
                    q.get()
                break
            else:
                if len(node.chilist) != 0:
                    for i in range(len(node.chilist)-1 ,- 1 ,- 1):
                        q.put(node.chilist[i])
                else:
                    continue
        if mindate.__ge__(date):
            mindate=date
            index_of_min=i
    return index_of_min

#更新库存
def ClStore():
    obj=Remain.objects.filter(Q(资材库存__gt=0)|Q(工序库存__gt=0)).order_by('物料号')
    for i in range(0,len(obj)):
        name = obj[i].名称
        print(name)
        index=EnsureTree(name) #开工日期早先拿到库存
        root=Tree_List[index]
        sremain=obj[i].工序库存+obj[i].资材库存
        q=LifoQueue(maxsize=10) # 后进先出
        q.put(root)
        while q.empty() != True:
            node=q.get()
            if node.name == name:
                if (node.number>sremain) :
                    node.number=node.number-sremain
                    DFS(node) #深度优先遍历
                    while not q.empty():
                        q.get()
                    break
                else:
                    sremain = sremain - node.number
                    node.number=0
                    DFS(node)
                    while q.empty() != True:
                        n_node=q.get()
                        if(n_node.name == name):
                            sremain = sremain-n_node.number
                            n_node.number=0
                            DFS(n_node)
                    q.put(Tree_List[len(Tree_List)-index-1])
                    continue



def show(request):
    ERP_List = []
    if request.method == 'GET':
        Need_List = models.Input.objects.all()

        for i in range(0, len(Need_List)):
            ClStore() #更新库存

        for i in range(0,len(Tree_list)):
            root = Tree_list[i]
            q=Queue()
            q.put(root)
            while q.empty() != True:
                Need = q.get()
                if Need.number != 0:
                    ERP_List.append(Need)
                if len(Need.chilist) != 0:
                    for i in range(0,len(Need.chilist)):
                        q.put(Need.chilist[i])

        ERP_List.sort(key=lambda TreeNode:TreeNode.predate)


        return render(request,'app01/input.html', {'Need_list':Need_List,'ERP_list':ERP_List})

    elif request.method == "POST":  # 放入数据库并且展示
        product_name = request.POST.get("product_name", None)
        product_number = request.POST.get("product_number", None)
        product_date = request.POST.get("product_date", None)


        models.Input.objects.create(
        产品名称=product_name,
        数量=product_number,
        完工日期=product_date,
        )
        Need_List = models.Input.objects.all()
        return render(request, "app01/input.html", {"Need_List":Need_List})

    return redirect('http://127.0.0.1:8000/app01/input')

    # 可以尝试以下这种方式，随时输入，随时输出而不是直接输出数据库的内容
    # elif request.method == 'POST':
    #     name = request.POST.get('name')
    #     number = int(request.POST.get('number'))
    #     date = request.POST.get('date')

    #     Mat = MatNeed(name, number, date)
    #     Need_list.append(Mat)

    #     return render(request, 'home.html', {'Need_list': Need_list})
