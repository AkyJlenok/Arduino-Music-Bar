"""
   Автор: AkyJlenok(R2BLZ)
   Код был написанн мной, из сборной солянки от других авторов. На совершенность или
   оптимизированность не претендую. Делал так, что бы работало

   73!
"""


import asyncio
import os
import serial.tools.list_ports
import serial
from time import sleep

from winsdk.windows.media.control import GlobalSystemMediaTransportControlsSessionManager as MediaManager


def transliterate(name):
   """
   Автор: LarsKort
   Дата: 16/07/2011; 1:05 GMT-4;
   Не претендую на "хорошесть" словарика. В моем случае и такой пойдет,
   вы всегда сможете добавить свои символы и даже слова. Только
   это нужно делать в обоих списках, иначе будет ошибка.
   """
   # Слоаврь с заменами
   slovar = {'а':'a','б':'b','в':'v','г':'g','д':'d','е':'e','ё':'e',
      'ж':'zh','з':'z','и':'i','й':'i','к':'k','л':'l','м':'m','н':'n',
      'о':'o','п':'p','р':'r','с':'s','т':'t','у':'u','ф':'f','х':'h',
      'ц':'c','ч':'cz','ш':'sh','щ':'scz','ъ':'','ы':'y','ь':'','э':'e',
      'ю':'u','я':'ja', 'А':'A','Б':'B','В':'V','Г':'G','Д':'D','Е':'E','Ё':'E',
      'Ж':'ZH','З':'Z','И':'I','Й':'I','К':'K','Л':'L','М':'M','Н':'N',
      'О':'O','П':'P','Р':'R','С':'S','Т':'T','У':'U','Ф':'F','Х':'H',
      'Ц':'C','Ч':'CZ','Ш':'SH','Щ':'SCH','Ъ':'','Ы':'y','Ь':'','Э':'E',
      'Ю':'U','Я':'YA',',':'','?':'',' ':' ','~':'','!':'','@':'@','#':'',
      '$':'$','%':'','^':'','&':'','*':'','(':'',')':'','-':'','=':'','+':'',
      ':':'',';':';','<':'','>':'','\'':'','"':'','\\':'','/':'/','№':'',
      '[':'',']':'','{':'','}':'','ґ':'','ї':'', 'є':'','Ґ':'g','Ї':'i',
      'Є':'e', '—':''}
        
   # Циклически заменяем все буквы в строке
   for key in slovar:
      name = name.replace(key, slovar[key])
   return name

any_app = False

async def get_media_info():

    sessions = await MediaManager.request_async()


    current_session = sessions.get_current_session()
    
    if current_session:
        if (current_session.source_app_user_model_id == 'A025C540.Yandex.Music_vfvw9svesycw6!App') or (any_app):         #Для получения id приложения используйте current_session.source_app_user_model_id, во время включённого источника звука
            info = await current_session.try_get_media_properties_async()

                    # song_attr[0] != '_' ignores system attributes
            info_dict = {song_attr: info.__getattribute__(song_attr) for song_attr in dir(info) if song_attr[0] != '_'}

                    # converts winrt vector to list
            info_dict['genres'] = list(info_dict['genres'])

            return info_dict
        raise Exception('TARGET_PROGRAM is not the current media session')
    # It could be possible to select a program from a list of current
    # available ones. I just haven't implemented this here for my use case.
    # See references for more information.


if input('Use Yandex Music or Any programm\n\r[0] <- Yandex Music\n\r[1] <- Any Programm\n\r') == '1':
    any_app = True;


ports = serial.tools.list_ports.comports()
for port in ports:
    print(port.device);

target_port = 'COM' + str(input('----Введите номер порта----\n'))

ser = serial.Serial(target_port, baudrate=9600)

sleep(2.5)

current_media_info = asyncio.run(get_media_info())
buffer = transliterate(f'${current_media_info['title']}@{current_media_info['artist']};')
ser.write(bytes(buffer, 'UTF-8'))

old_title = ''

while True:
    current_media_info = asyncio.run(get_media_info())

    if current_media_info['title'] != old_title:
        os.system('cls')
        print(f'---{current_media_info['title']}---\n\r--{current_media_info['artist']}--')
        old_title = current_media_info['title']
    
        buffer = transliterate(f'${current_media_info['title']}@{current_media_info['artist']};')

        ser.write(bytes(buffer, 'UTF-8'))
    sleep(1)
