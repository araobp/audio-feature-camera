# Generate dataset for Keras/TensorFlow

import yaml
import numpy as np
import sklearn.preprocessing as pp
import random
import os
import time
import glob
from keras.utils import to_categorical
from keras import models

class DataSet:
    '''
     - /dataset_folder --+-- /data/*.csv
                         |
                         +-- /dataset.yaml
                         |
                         +-- /class_labels.yaml
                         |
                         +-- /*.h5
    '''

    def __init__(self, dataset_folder):

        self.dataset_folder = dataset_folder    
        with open(dataset_folder + '/dataset.yaml') as f:
            attr = yaml.load(f)
        self.filters = attr['filters']
        self.files = attr['files']
        self.samples = attr['samples']
        self.length = attr['length']
        self.training_files = attr['training_files']
        self.test_files = self.files - self.training_files
        self.feature = attr['feature']
        self.stride = attr['stride']
        self.cutoff = attr['cutoff']
        if attr['model']:
            self.model = models.load_model(dataset_folder + '/' + attr['model'])
        else:
            self.model = None
        
        if not self.cutoff:
            self.cutoff = self.filters

        if self.feature == 'mfcc':
            self.shape = (self.length, self.cutoff-1)  # DC removed
        else:
            self.shape = (self.length, self.cutoff)

        # Generate windows
        windows = []
        a, b, i = 0, 0, 0
        while True:
            a, b = self.stride*i, self.stride*i+self.length
            if b > 200:
                break
            windows.append([a, b, self.cutoff])
            i += 1
        self.windows = windows

        # Note: "class_labels.yaml" is generated by generate() method
        self.class_labels = None
        class_labels_file = dataset_folder + '/class_labels.yaml'
        if os.path.isfile(class_labels_file):
            with open(class_labels_file, 'r') as f:
                self.class_labels = yaml.load(f)
            
    def generate(self, flatten=False):
        '''
        Generate training data set and test data set for Keras/TensorFlow
        '''

        feature_length = 200 * self.filters
        
        data_files = glob.glob(self.dataset_folder+'/data/*-features-*.csv')
        class_labels = []

        for file in data_files:
            label = file.split('-')[2]
            if label not in class_labels:
                class_labels.append(label)
                
        data_set = {}
        class_number = 0

        for label in class_labels:
            files = glob.glob(self.dataset_folder+'/data/*-*-{}-*.csv'.format(label))
            random.shuffle(files)
            data_set[label] = (files[:self.training_files], files[self.training_files:self.files], class_number)
            class_number += 1

        training_set = []
        test_set = []
                    
        for k, v in data_set.items():
            files = v[0]
            class_number = v[2]
            for file in files:
                params = file.split('-')
                pos = params[3]
                if pos != 'a':
                    try:
                        pos = int(pos)
                    except:
                        pos = 0
                with open(file) as f:
                    data = np.array(f.read().split(',')).astype(float)
                    if self.feature == 'mel_spectrogram':
                        data = data[:feature_length]
                    elif self.feature == 'mfcc':
                        data = data[feature_length:]
                    if type(pos) == int:
                        w = self.windows[pos]
                        img = pp.scale(data[w[0]*self.filters:w[1]*self.filters])
                        training_set.append((img, class_number))                        
                    else:
                        for w in self.windows:
                            img = pp.scale(data[w[0]*self.filters:w[1]*self.filters])
                            training_set.append((img, class_number))
            files = v[1]
            for file in files:
                params = file.split('-')
                pos = params[3]
                if pos != 'a':
                    pos = int(pos)
                with open(file) as f:
                    data = np.array(f.read().split(',')).astype(float)
                    if self.feature == 'mel_spectrogram':
                        data = data[:feature_length]
                    elif self.feature == 'mfcc':
                        data = data[feature_length:]
                    if type(pos) == int:
                        w = self.windows[pos]
                        img = pp.scale(data[w[0]*self.filters:w[1]*self.filters])
                        test_set.append((img, class_number))                        
                    else:
                        for w in self.windows:
                            img = pp.scale(data[w[0]*self.filters:w[1]*self.filters])
                            test_set.append((img, class_number))

        random.shuffle(training_set)
        random.shuffle(test_set)

        self.class_labels = [None for _ in range(len(data_set))]

        # Class number and class labels
        for k,v in data_set.items():
            self.class_labels[v[2]] = k
            
        with open(self.dataset_folder+'/class_labels.yaml', 'w') as f:
            yaml.dump(self.class_labels, f)

        train_data, train_labels = [], []
        test_data, test_labels = [], []

        for img, label in training_set:
            train_data.append(img)
            train_labels.append(label)

        for img, label in test_set:
            test_data.append(img)
            test_labels.append(label)
        
        if pos == 'a':
            len_windows = len(self.windows)
        else:
            len_windows = 1
       
        train_data = np.array(train_data, dtype='float32').reshape((self.training_files*len(class_labels)*len_windows, self.length, self.filters, 1))
        if self.feature == 'mfcc':
            train_data = train_data[:,:,1:self.cutoff,:]  # Remove DC
        else:
            train_data = train_data[:,:,0:self.cutoff,:]            
        if flatten:
            train_data = train_data.reshape((train_data.shape[0], -1))
        train_labels = np.array(train_labels, dtype='uint8')
       
        test_data = np.array(test_data, dtype='float32').reshape((self.test_files*len(class_labels)*len_windows, self.length, self.filters, 1))
        if self.feature == 'mfcc':
            test_data = test_data[:,:,1:self.cutoff,:]  # Remove DC
        else:
            test_data = test_data[:,:,0:self.cutoff,:]            
        if flatten:
            test_data = test_data.reshape((test_data.shape[0], -1))
        test_labels = np.array(test_labels, dtype='uint8')

        train_labels=to_categorical(train_labels)
        test_labels=to_categorical(test_labels)

        self.train_data = train_data
        self.train_labels = train_labels
        self.test_data = test_data
        self.test_labels = test_labels
        
        return (train_data, train_labels, test_data, test_labels)

    def count_class_labels(self):

        data_files = glob.glob(self.dataset_folder + '/data/*features*.csv')
        class_labels = {}

        for file in data_files:
            label = file.replace('\\', '/').split('/')[-1].split('-')[2]
            if label not in class_labels:
                class_labels[label] = 1
            else:
                class_labels[label] = class_labels[label] + 1

        return class_labels
