import random

dayoffset = 10
devicepool1 = [ 
    [ "webcam_FHD", "webcam_UHD" ],
    [ "projector_2K", "projector_4K" ]
]
devicepool2 = [
    [ "monitor_50", "monitor_75" ],
    [ "screen_100", "screen_150" ]
]
tenantpool = ["tenant_A", "tenant_B", "tenant_C", "tenant_D", "tenant_E"]

def gen_addMeeting():
    '''
    syntax: addMeeting -tenant YYYY-MM-DD hh:mm n.n p [d1 d2]
    '''
    # tenantpool
    # dayoffset
    # devicepool1
    # devicepool2
    rand_t = random.randint(0,len(tenantpool)-1)
    rand_d = random.randint(0,6)            # within one week
    rand_h = random.randint(0,23)           # within 24 hours
    rand_m = random.randrange(0,59,10)      # within 60 mins
    rand_n1 = random.randrange(0,3,1)       # within 2 hours
    rand_n2 = random.randrange(0,10,1)      # within 1.0 hour
    rand_p = random.randrange(5,10,2)       # 5,7,9 people
    rand_ifd = random.randint(0,2)          # 0 gowith 1st pair
                                            # 1 gowith 2nd pair
                                            # 2 no device
    rand_d1 = random.randint(0,1)
    rand_d2 = random.randint(0,1)
    if (rand_ifd != 2) :
        string = " ".join(
            [ 
                "addMeeting",
                "-" + str(tenantpool[rand_t]),
                "2021-05-" + str(dayoffset + rand_d),
                str(rand_h) + ":" + str(rand_m),
                str(rand_n1) + "." + str(rand_n2),
                str(rand_p),
                devicepool1[rand_ifd][rand_d1],
                devicepool2[rand_ifd][rand_d2]
            ]
        )
    else:
        string = " ".join(
            [ 
                "addMeeting",
                "-" + str(tenantpool[rand_t]),
                "2021-05-" + str(dayoffset + rand_d),
                str(rand_h) + ":" + str(rand_m),
                str(rand_n1) + "." + str(rand_n2),
                str(rand_p)
            ]
        )
    return(string + ";")
def gen_addPresentation():
    '''
    syntax: addPresentation -tenant YYYY-MM-DD hh:mm n.n p d1 d2
    '''
    # tenantpool
    # dayoffset
    # devicepool1
    # devicepool2
    rand_t = random.randint(0,len(tenantpool)-1)
    rand_d = random.randint(0,6)            # within one week
    rand_h = random.randint(0,23)           # within 24 hours
    rand_m = random.randrange(0,59,10)      # within 60 mins
    rand_n1 = random.randrange(0,4,1)       # within 3 hours
    rand_n2 = random.randrange(0,10,1)      # within 1.0 hour
    rand_p = random.randrange(10,15,5)      # 10,15 people
    rand_ifd = 1                            # must have projector&screen
    rand_d1 = random.randint(0,1)
    rand_d2 = random.randint(0,1)
    string = " ".join(
        [ 
            "addPresentation",
            "-" + str(tenantpool[rand_t]),
            "2021-05-" + str(dayoffset + rand_d),
            str(rand_h) + ":" + str(rand_m),
            str(rand_n1) + "." + str(rand_n2),
            str(rand_p),
            devicepool1[rand_ifd][rand_d1],
            devicepool2[rand_ifd][rand_d2]
        ]
    )
    return(string + ";")
def gen_addConference():
    '''
    syntax: addConference -tenant YYYY-MM-DD hh:mm n.n p d1 d2
    '''
    # tenantpool
    # dayoffset
    # devicepool1
    # devicepool2
    rand_t = random.randint(0,len(tenantpool)-1)
    rand_d = random.randint(0,6)            # within one week
    rand_h = random.randint(0,23)           # within 24 hours
    rand_m = random.randrange(0,59,10)      # within 60 mins
    rand_n1 = random.randrange(0,6,1)       # within 5 hours
    rand_n2 = random.randrange(0,10,1)      # within 1.0 hour
    rand_p = random.randrange(10,20,5)      # 10,15,20 people
    rand_ifd = 0
    rand_d1 = random.randint(0,1)
    rand_d2 = random.randint(0,1)
    string = " ".join(
        [ 
            "addConference",
            "-" + str(tenantpool[rand_t]),
            "2021-05-" + str(dayoffset + rand_d),
            str(rand_h) + ":" + str(rand_m),
            str(rand_n1) + "." + str(rand_n2),
            str(rand_p),
            devicepool1[rand_ifd][rand_d1],
            devicepool2[rand_ifd][rand_d2]
        ]
    )
    return(string + ";")
def gen_bookDevice():
    '''
    syntax: bookDevice -tenant YYYY-MM-DD hh:mm n.n d1
    '''
    # tenantpool
    # dayoffset
    # devicepool1
    # devicepool2
    rand_t = random.randint(0,len(tenantpool)-1)
    rand_d = random.randint(0,6)            # within one week
    rand_h = random.randint(0,23)           # within 24 hours
    rand_m = random.randrange(0,59,10)      # within 60 mins
    rand_n1 = random.randrange(0,3,1)       # within 2 hours
    rand_n2 = random.randrange(0,10,1)      # within 1.0 hour
    rand_ifd = random.randint(0,1)          # 0 gowith pool1
                                            # 1 gowith pool2
    rand_d1 = random.randint(0,1)
    rand_d2 = random.randint(0,1)
    if rand_ifd:
        string = " ".join(
            [ 
                "bookDevice",
                "-" + str(tenantpool[rand_t]),
                "2021-05-" + str(dayoffset + rand_d),
                str(rand_h) + ":" + str(rand_m),
                str(rand_n1) + "." + str(rand_n2),
                devicepool1[rand_d1][rand_d2],
            ]
        )
    else:
        string = " ".join(
            [ 
                "bookDevice",
                "-" + str(tenantpool[rand_t]),
                "2021-05-" + str(dayoffset + rand_d),
                str(rand_h) + ":" + str(rand_m),
                str(rand_n1) + "." + str(rand_n2),
                devicepool2[rand_d1][rand_d2],
            ]
        )
    return(string + ";")

for i in range(100):
    print(gen_addMeeting())
# for i in range(100):
#     print(gen_addConference())
# for i in range(100):
#     print(gen_addPresentation())
# for i in range(100):
#     print(gen_bookDevice())